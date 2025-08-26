#include "neocpp/wallet/bip39_account.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/crypto/nep2.hpp"
#include "neocpp/crypto/wif.hpp"
#include <stdexcept>
#include <sstream>

namespace neocpp {

SharedPtr<Bip39Account> Bip39Account::create(
    const std::string& password,
    Bip39::Strength strength,
    Bip39::Language language,
    const std::string& label) {
    
    try {
        // Generate new mnemonic phrase
        std::string mnemonic = Bip39::generateMnemonic(strength, language);
        
        // Validate the generated mnemonic
        if (!Bip39::validateMnemonic(mnemonic, language)) {
            throw std::runtime_error("Generated mnemonic is invalid");
        }
        
        // Derive private key from mnemonic and password
        Bytes privateKeyBytes = derivePrivateKey(mnemonic, password);
        
        // Create EC key pair from private key
        auto keyPair = std::make_shared<ECKeyPair>(privateKeyBytes);
        
        // Create and return the BIP39 account
        return SharedPtr<Bip39Account>(new Bip39Account(keyPair, mnemonic, label));
        
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Failed to create BIP39 account: " << e.what();
        throw std::runtime_error(ss.str());
    }
}

SharedPtr<Bip39Account> Bip39Account::fromMnemonic(
    const std::string& password,
    const std::string& mnemonic,
    Bip39::Language language,
    const std::string& label) {
    
    try {
        // Validate the mnemonic phrase
        if (!Bip39::validateMnemonic(mnemonic, language)) {
            throw std::runtime_error("Invalid mnemonic phrase");
        }
        
        // Derive private key from mnemonic and password
        Bytes privateKeyBytes = derivePrivateKey(mnemonic, password);
        
        // Create EC key pair from private key
        auto keyPair = std::make_shared<ECKeyPair>(privateKeyBytes);
        
        // Create and return the BIP39 account
        return SharedPtr<Bip39Account>(new Bip39Account(keyPair, mnemonic, label));
        
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Failed to recover BIP39 account from mnemonic: " << e.what();
        throw std::runtime_error(ss.str());
    }
}

Bytes Bip39Account::derivePrivateKey(const std::string& mnemonic, const std::string& password) {
    // Generate seed from mnemonic and password
    // BIP39 uses password as passphrase for seed generation
    Bytes seed = Bip39::mnemonicToSeed(mnemonic, password);
    
    // For Neo, we derive the private key by taking SHA256 of the seed
    // This follows the Neo-specific key derivation: Key = SHA-256(BIP_39_SEED(mnemonic, password))
    return Hash::sha256(seed);
}

std::string Bip39Account::exportNEP2(const std::string& password) const {
    if (!getKeyPair()) {
        throw std::runtime_error("No private key available for NEP2 export");
    }
    
    try {
        // Get the private key bytes
        Bytes privateKeyBytes = getKeyPair()->getPrivateKeyBytes();
        
        // Encrypt using NEP2 format
        return NEP2::encrypt(privateKeyBytes, password);
        
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Failed to export account to NEP2: " << e.what();
        throw std::runtime_error(ss.str());
    }
}

std::string Bip39Account::exportWIF() const {
    if (!getKeyPair()) {
        throw std::runtime_error("No private key available for WIF export");
    }
    
    if (isLocked()) {
        throw std::runtime_error("Account is locked, cannot export WIF");
    }
    
    try {
        // Get the private key bytes
        Bytes privateKeyBytes = getKeyPair()->getPrivateKeyBytes();
        
        // Encode as WIF
        return WIF::encode(privateKeyBytes);
        
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Failed to export account to WIF: " << e.what();
        throw std::runtime_error(ss.str());
    }
}

} // namespace neocpp