#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/crypto/nep2.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

Account::Account(const SharedPtr<ECKeyPair>& keyPair, const std::string& label)
    : label_(label),
      keyPair_(keyPair),
      isDefault_(false),
      isLocked_(false) {
    scriptHash_ = Hash160::fromPublicKey(keyPair->getPublicKey()->getEncoded());
    address_ = scriptHash_.toAddress();
}

Account::Account(const std::string& wif, const std::string& label)
    : Account(std::make_shared<ECKeyPair>(ECKeyPair::fromWIF(wif)), label) {
}

Account::Account(const std::string& nep2, const std::string& password, const std::string& label)
    : label_(label),
      isDefault_(false),
      isLocked_(true),
      encryptedPrivateKey_(nep2) {
    // Decrypt to get address
    auto tempKeyPair = std::make_shared<ECKeyPair>(NEP2::decryptToKeyPair(nep2, password));
    scriptHash_ = Hash160::fromPublicKey(tempKeyPair->getPublicKey()->getEncoded());
    address_ = scriptHash_.toAddress();
    // Don't store decrypted key
}

Account::Account(const std::vector<SharedPtr<ECPublicKey>>& publicKeys, int signingThreshold, const std::string& label)
    : label_(label),
      isDefault_(false),
      isLocked_(false) {
    if (signingThreshold <= 0 || signingThreshold > publicKeys.size()) {
        throw IllegalArgumentException("Invalid signing threshold");
    }
    
    scriptHash_ = Hash160::fromPublicKeys(publicKeys, signingThreshold);
    address_ = scriptHash_.toAddress();
    // For multi-sig, we don't have a single key pair
    keyPair_ = nullptr;
}

void Account::lock(const std::string& password) {
    if (!keyPair_) {
        throw WalletException("Cannot lock multi-signature account");
    }
    
    if (isLocked_) {
        return;
    }
    
    encryptedPrivateKey_ = NEP2::encrypt(*keyPair_, password);
    keyPair_ = nullptr;
    isLocked_ = true;
}

bool Account::unlock(const std::string& password) {
    if (!isLocked_) {
        return true;
    }
    
    if (encryptedPrivateKey_.empty()) {
        return false;
    }
    
    try {
        keyPair_ = std::make_shared<ECKeyPair>(NEP2::decryptToKeyPair(encryptedPrivateKey_, password));
        isLocked_ = false;
        return true;
    } catch (const NEP2Exception&) {
        // Invalid password or corrupted encrypted key
        return false;
    } catch (const CryptoException&) {
        // Other crypto errors during decryption
        return false;
    } catch (const std::exception&) {
        // Other unexpected errors
        return false;
    }
}

bool Account::isMultiSig() const {
    return keyPair_ == nullptr && !isLocked_;
}

Bytes Account::getVerificationScript() const {
    if (keyPair_) {
        return ScriptBuilder::buildVerificationScript(keyPair_->getPublicKey());
    }
    
    if (contract_) {
        return contract_->getScript();
    }
    
    return Bytes();
}

Bytes Account::sign(const Bytes& message) const {
    if (isLocked_) {
        throw WalletException("Account is locked");
    }
    
    if (!keyPair_) {
        throw WalletException("Cannot sign with multi-signature account");
    }
    
    auto signature = keyPair_->sign(message);
    return signature->getBytes();
}

bool Account::verify(const Bytes& message, const Bytes& signature) const {
    if (!keyPair_) {
        return false;
    }
    
    auto sig = std::make_shared<ECDSASignature>(signature);
    return keyPair_->getPublicKey()->verify(message, sig);
}

std::string Account::exportWIF() const {
    if (isLocked_ || !keyPair_) {
        return "";
    }
    
    return keyPair_->exportAsWIF();
}

std::string Account::exportNEP2(const std::string& password) const {
    if (!keyPair_) {
        throw WalletException("Cannot export multi-signature account");
    }
    
    if (isLocked_ && !encryptedPrivateKey_.empty()) {
        return encryptedPrivateKey_;
    }
    
    return NEP2::encrypt(*keyPair_, password);
}

SharedPtr<Account> Account::create(const std::string& label) {
    auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::generate());
    return std::make_shared<Account>(keyPair, label);
}

SharedPtr<Account> Account::fromWIF(const std::string& wif, const std::string& label) {
    auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::fromWIF(wif));
    return std::make_shared<Account>(keyPair, label);
}

SharedPtr<Account> Account::fromNEP2(const std::string& nep2, const std::string& password, const std::string& label) {
    return std::make_shared<Account>(nep2, password, label);
}

} // namespace neocpp