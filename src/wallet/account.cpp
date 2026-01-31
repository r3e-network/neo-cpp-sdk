
#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/crypto/nep2.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"

namespace neocpp {

Account::Account(const SharedPtr<ECKeyPair>& keyPair, const std::string& label)
    : label_(label),
      keyPair_(keyPair),
      isDefault_(false),
      isLocked_(false) {
    scriptHash_ = Hash160::fromPublicKey(keyPair->getPublicKey()->getEncoded());
    address_ = scriptHash_.toAddress();
} // namespace neocpp
Account::Account(const std::string& wif, const std::string& label)
    : Account(std::make_shared<ECKeyPair>(ECKeyPair::fromWIF(wif)), label) {
} // namespace neocpp
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
} // namespace neocpp
Account::Account(const std::vector<SharedPtr<ECPublicKey>>& publicKeys, int signingThreshold, const std::string& label)
    : label_(label),
      isDefault_(false),
      isLocked_(false) {
    if (signingThreshold <= 0 || static_cast<size_t>(signingThreshold) > publicKeys.size()) {
        throw IllegalArgumentException("Invalid signing threshold");
    }

    scriptHash_ = Hash160::fromPublicKeys(publicKeys, signingThreshold);
    address_ = scriptHash_.toAddress();
    // For multi-sig, we don't have a single key pair
    keyPair_ = nullptr;
} // namespace neocpp
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
} // namespace neocpp
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
    } catch (const NEP2Exception& e) {
        LOG_DEBUG(std::string("NEP2 decryption failed: ") + e.what());
        return false;
    } catch (const CryptoException& e) {
        LOG_DEBUG(std::string("Crypto error during account unlock: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("Unexpected error during account unlock: ") + e.what());
        return false;
    }
} // namespace neocpp
bool Account::isMultiSig() const {
    return keyPair_ == nullptr && !isLocked_;
} // namespace neocpp
Bytes Account::getVerificationScript() const {
    if (keyPair_) {
        return ScriptBuilder::buildVerificationScript(keyPair_->getPublicKey());
    }

    if (contract_) {
        return contract_->getScript();
    }

    return Bytes();
} // namespace neocpp
Bytes Account::sign(const Bytes& message) const {
    if (isLocked_) {
        throw WalletException("Account is locked");
    }

    if (!keyPair_) {
        throw WalletException("Cannot sign with multi-signature account");
    }

    auto signature = keyPair_->sign(message);
    return signature->getBytes();
} // namespace neocpp
bool Account::verify(const Bytes& message, const Bytes& signature) const {
    if (!keyPair_) {
        return false;
    }

    auto sig = std::make_shared<ECDSASignature>(signature);
    return keyPair_->getPublicKey()->verify(message, sig);
} // namespace neocpp
std::string Account::exportWIF() const {
    if (isLocked_ || !keyPair_) {
        return "";
    }

    return keyPair_->exportAsWIF();
} // namespace neocpp
std::string Account::exportNEP2(const std::string& password) const {
    if (!keyPair_) {
        throw WalletException("Cannot export multi-signature account");
    }

    if (isLocked_ && !encryptedPrivateKey_.empty()) {
        return encryptedPrivateKey_;
    }

    return NEP2::encrypt(*keyPair_, password);
} // namespace neocpp
SharedPtr<Account> Account::create(const std::string& label) {
    auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::generate());
    return std::make_shared<Account>(keyPair, label);
} // namespace neocpp
SharedPtr<Account> Account::fromWIF(const std::string& wif, const std::string& label) {
    auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::fromWIF(wif));
    return std::make_shared<Account>(keyPair, label);
} // namespace neocpp
SharedPtr<Account> Account::fromNEP2(const std::string& nep2, const std::string& password, const std::string& label) {
    return std::make_shared<Account>(nep2, password, label);
} // namespace neocpp
SharedPtr<Account> Account::fromPrivateKey(const Bytes& privateKey, const std::string& label) {
    auto keyPair = std::make_shared<ECKeyPair>(privateKey);
    return std::make_shared<Account>(keyPair, label);
} // namespace neocpp
SharedPtr<Account> Account::fromAddress(const std::string& address, const std::string& label) {
    auto account = std::make_shared<Account>();
    account->address_ = address;
    account->scriptHash_ = Hash160::fromAddress(address);
    account->label_ = label;
    account->isDefault_ = false;
    account->isLocked_ = false;
    account->keyPair_ = nullptr;  // Watch-only, no private key
    return account;
} // namespace neocpp
} // namespace neocpp
