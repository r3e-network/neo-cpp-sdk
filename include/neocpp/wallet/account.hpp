#pragma once

#include <string>
#include <memory>
#include <vector>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/contract/contract.hpp"

namespace neocpp {

// Forward declarations
class ECKeyPair;
class ECPublicKey;

/// Represents a Neo account
class Account {
private:
    std::string label_;
    std::string address_;
    Hash160 scriptHash_;
    SharedPtr<ECKeyPair> keyPair_;
    SharedPtr<Contract> contract_;
    bool isDefault_;
    bool isLocked_;
    std::string encryptedPrivateKey_;  // NEP-2 encrypted

public:
    /// Create account from key pair
    /// @param keyPair The EC key pair
    /// @param label Optional label for the account
    explicit Account(const SharedPtr<ECKeyPair>& keyPair, const std::string& label = "");

    /// Create account from WIF
    /// @param wif The WIF-encoded private key
    /// @param label Optional label for the account
    explicit Account(const std::string& wif, const std::string& label = "");

    /// Create account from NEP-2 encrypted key
    /// @param nep2 The NEP-2 encrypted private key
    /// @param password The password to decrypt
    /// @param label Optional label for the account
    Account(const std::string& nep2, const std::string& password, const std::string& label = "");

    /// Create multi-signature account
    /// @param publicKeys The public keys
    /// @param signingThreshold The minimum number of signatures required
    /// @param label Optional label for the account
    Account(const std::vector<SharedPtr<ECPublicKey>>& publicKeys, int signingThreshold, const std::string& label = "");

    /// Default constructor (for internal use)
    Account() = default;

    /// Destructor
    ~Account() = default;

    // Getters
    const std::string& getLabel() const { return label_; }
    const std::string& getAddress() const { return address_; }
    const Hash160& getScriptHash() const { return scriptHash_; }
    const SharedPtr<ECKeyPair>& getKeyPair() const { return keyPair_; }
    const SharedPtr<Contract>& getContract() const { return contract_; }
    [[nodiscard]] bool getIsDefault() const { return isDefault_; }
    [[nodiscard]] bool getIsLocked() const { return isLocked_; }
    const std::string& getEncryptedPrivateKey() const { return encryptedPrivateKey_; }

    // Setters
    void setLabel(const std::string& label) { label_ = label; }
    void setIsDefault(bool isDefault) { isDefault_ = isDefault; }

    /// Lock the account (encrypt private key)
    /// @param password The password to use for encryption
    void lock(const std::string& password);

    /// Unlock the account (decrypt private key)
    /// @param password The password to use for decryption
    /// @return True if successfully unlocked
    bool unlock(const std::string& password);

    /// Check if account is locked
    /// @return True if locked
    [[nodiscard]] bool isLocked() const { return isLocked_; }

    /// Check if this is a multi-signature account
    /// @return True if multi-sig
    [[nodiscard]] bool isMultiSig() const;

    /// Get the verification script
    /// @return The verification script
    [[nodiscard]] Bytes getVerificationScript() const;

    /// Sign a message
    /// @param message The message to sign
    /// @return The signature
    Bytes sign(const Bytes& message) const;

    /// Verify a signature
    /// @param message The message that was signed
    /// @param signature The signature to verify
    /// @return True if signature is valid
    [[nodiscard]] bool verify(const Bytes& message, const Bytes& signature) const;

    /// Export private key as WIF
    /// @return The WIF-encoded private key (empty if locked)
    std::string exportWIF() const;

    /// Export as NEP-2 encrypted key
    /// @param password The password for encryption
    /// @return The NEP-2 encrypted private key
    std::string exportNEP2(const std::string& password) const;

    /// Create a new random account
    /// @param label Optional label for the account
    /// @return The new account
    static SharedPtr<Account> create(const std::string& label = "");

    /// Import account from WIF
    /// @param wif The WIF-encoded private key
    /// @param label Optional label for the account
    /// @return The imported account
    static SharedPtr<Account> fromWIF(const std::string& wif, const std::string& label = "");

    /// Import account from NEP-2
    /// @param nep2 The NEP-2 encrypted private key
    /// @param password The password to decrypt
    /// @param label Optional label for the account
    /// @return The imported account
    static SharedPtr<Account> fromNEP2(const std::string& nep2, const std::string& password, const std::string& label = "");

    /// Import account from private key bytes
    /// @param privateKey The private key bytes (32 bytes)
    /// @param label Optional label for the account
    /// @return The imported account
    static SharedPtr<Account> fromPrivateKey(const Bytes& privateKey, const std::string& label = "");

    /// Import account from address (watch-only)
    /// @param address The Neo address
    /// @param label Optional label for the account
    /// @return The imported account (watch-only, no private key)
    static SharedPtr<Account> fromAddress(const std::string& address, const std::string& label = "");
};

} // namespace neocpp
