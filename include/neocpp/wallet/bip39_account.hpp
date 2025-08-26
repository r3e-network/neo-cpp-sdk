#ifndef NEOCPP_WALLET_BIP39_ACCOUNT_HPP
#define NEOCPP_WALLET_BIP39_ACCOUNT_HPP

#include <string>
#include <memory>
#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/bip39.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"

namespace neocpp {

/**
 * Class encapsulating a BIP-39 compatible NEO account.
 * 
 * BIP39 accounts use mnemonic phrases for key generation and recovery.
 * The private key is derived using: Key = SHA-256(BIP_39_SEED(mnemonic, password))
 */
class Bip39Account : public Account {
private:
    std::string mnemonic_;
    
    /// Private constructor
    Bip39Account(const SharedPtr<ECKeyPair>& keyPair, const std::string& mnemonic, const std::string& label = "")
        : Account(keyPair, label), mnemonic_(mnemonic) {}
    
public:
    /**
     * Generate a new BIP-39 compatible NEO account.
     * 
     * The private key for the wallet is calculated using:
     * Key = SHA-256(BIP_39_SEED(mnemonic, password))
     * 
     * The password is used as passphrase for BIP-39 seed generation
     * and will be needed to recover the account later.
     * 
     * @param password The passphrase with which to encrypt the private key
     * @param strength The mnemonic strength (default: 128 bits = 12 words)
     * @param language The mnemonic language (default: English)
     * @param label Optional account label
     * @return A new BIP-39 compatible Neo account
     * @throws std::runtime_error if account creation fails
     */
    static SharedPtr<Bip39Account> create(
        const std::string& password,
        Bip39::Strength strength = Bip39::Strength::ENTROPY_128,
        Bip39::Language language = Bip39::Language::ENGLISH,
        const std::string& label = "");
    
    /**
     * Generate a new BIP-39 compatible NEO account with default settings.
     * 
     * Creates an account with 128-bit entropy (12 words) in English.
     * 
     * @param password The passphrase with which to encrypt the private key
     * @param label Optional account label
     * @return A new BIP-39 compatible Neo account
     * @throws std::runtime_error if account creation fails
     */
    static SharedPtr<Bip39Account> create(
        const std::string& password,
        const std::string& label) {
        return create(password, Bip39::Strength::ENTROPY_128, Bip39::Language::ENGLISH, label);
    }
    
    /**
     * Recover a BIP-39 account from mnemonic phrase and password.
     * 
     * @param password The passphrase given when the BIP-39 account was generated
     * @param mnemonic The generated mnemonic phrase
     * @param language The mnemonic language (default: English)
     * @param label Optional account label
     * @return A recovered BIP-39 compatible Neo account
     * @throws std::runtime_error if mnemonic is invalid or recovery fails
     */
    static SharedPtr<Bip39Account> fromMnemonic(
        const std::string& password,
        const std::string& mnemonic,
        Bip39::Language language = Bip39::Language::ENGLISH,
        const std::string& label = "");
    
    /**
     * Get the BIP-39 mnemonic phrase for this account.
     * 
     * @return The mnemonic phrase (space-separated words)
     */
    const std::string& getMnemonic() const { return mnemonic_; }
    
    /**
     * Get the mnemonic words as a vector.
     * 
     * @return Vector of mnemonic words
     */
    std::vector<std::string> getMnemonicWords() const {
        return Bip39::splitMnemonic(mnemonic_);
    }
    
    /**
     * Validate the mnemonic phrase.
     * 
     * @param language The mnemonic language (default: English)
     * @return True if the mnemonic is valid
     */
    bool validateMnemonic(Bip39::Language language = Bip39::Language::ENGLISH) const {
        return Bip39::validateMnemonic(mnemonic_, language);
    }
    
    /**
     * Export account to NEP-2 format with encryption.
     * 
     * @param password The password for NEP-2 encryption
     * @return The NEP-2 encrypted private key
     * @throws std::runtime_error if encryption fails
     */
    std::string exportNEP2(const std::string& password) const;
    
    /**
     * Export account to WIF format.
     * 
     * WARNING: This exports the unencrypted private key.
     * 
     * @return The WIF-encoded private key
     * @throws std::runtime_error if the account is locked
     */
    std::string exportWIF() const;
    
    /**
     * Check if this is a BIP-39 account.
     * 
     * @return Always returns true for Bip39Account
     */
    virtual bool isBip39() const { return true; }
    
    /**
     * Get account type string.
     * 
     * @return "BIP39"
     */
    virtual std::string getType() const { return "BIP39"; }
    
    /// Destructor
    virtual ~Bip39Account() = default;
    
private:
    /**
     * Derive private key from mnemonic and password.
     * 
     * @param mnemonic The mnemonic phrase
     * @param password The passphrase
     * @return The derived private key bytes
     */
    static Bytes derivePrivateKey(const std::string& mnemonic, const std::string& password);
};

} // namespace neocpp

#endif // NEOCPP_WALLET_BIP39_ACCOUNT_HPP