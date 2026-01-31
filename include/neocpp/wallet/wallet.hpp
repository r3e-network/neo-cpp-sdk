#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

// Forward declarations
class Account;
class Transaction;

/// Represents a Neo wallet
class Wallet {
protected:
    std::string name_;
    std::string version_;
    std::vector<SharedPtr<Account>> accounts_;
    std::unordered_map<std::string, SharedPtr<Account>> accountsByAddress_;
    std::unordered_map<Hash160, SharedPtr<Account>, Hash160::Hasher> accountsByScriptHash_;

public:
    /// Constructor
    /// @param name The wallet name
    /// @param version The wallet version
    explicit Wallet(const std::string& name = "NeoCpp Wallet", const std::string& version = "1.0");

    /// Destructor
    virtual ~Wallet() = default;

    // Getters
    const std::string& getName() const { return name_; }
    const std::string& getVersion() const { return version_; }
    const std::vector<SharedPtr<Account>>& getAccounts() const { return accounts_; }

    // Setters
    void setName(const std::string& name) { name_ = name; }
    void setVersion(const std::string& version) { version_ = version; }

    /// Add an account to the wallet
    /// @param account The account to add
    void addAccount(const SharedPtr<Account>& account);

    /// Remove an account from the wallet
    /// @param address The address of the account to remove
    /// @return True if removed successfully
    bool removeAccount(const std::string& address);

    /// Get an account by address
    /// @param address The account address
    /// @return The account or nullptr if not found
    SharedPtr<Account> getAccount(const std::string& address) const;

    /// Get an account by script hash
    /// @param scriptHash The account script hash
    /// @return The account or nullptr if not found
    SharedPtr<Account> getAccount(const Hash160& scriptHash) const;

    /// Get the default account
    /// @return The default account or nullptr if none set
    SharedPtr<Account> getDefaultAccount() const;

    /// Set the default account
    /// @param address The address of the account to set as default
    /// @return True if set successfully
    bool setDefaultAccount(const std::string& address);

    /// Check if wallet contains an account
    /// @param address The account address
    /// @return True if wallet contains the account
    bool containsAccount(const std::string& address) const;

    /// Check if wallet contains an account
    /// @param scriptHash The account script hash
    /// @return True if wallet contains the account
    bool containsAccount(const Hash160& scriptHash) const;

    /// Create a new account in the wallet
    /// @param label Optional label for the account
    /// @return The created account
    SharedPtr<Account> createAccount(const std::string& label = "");

    /// Import an account from WIF
    /// @param wif The WIF-encoded private key
    /// @param label Optional label for the account
    /// @return The imported account
    SharedPtr<Account> importFromWIF(const std::string& wif, const std::string& label = "");

    /// Import an account from NEP-2
    /// @param nep2 The NEP-2 encrypted private key
    /// @param password The password to decrypt
    /// @param label Optional label for the account
    /// @return The imported account
    SharedPtr<Account> importFromNEP2(const std::string& nep2, const std::string& password, const std::string& label = "");

    /// Sign a transaction with available accounts
    /// @param transaction The transaction to sign
    /// @return True if signed successfully
    bool signTransaction(const SharedPtr<Transaction>& transaction);

    /// Get the number of accounts
    /// @return The account count
    [[nodiscard]] size_t size() const { return accounts_.size(); }

    /// Check if wallet is empty
    /// @return True if no accounts
    [[nodiscard]] bool isEmpty() const { return accounts_.empty(); }

    /// Clear all accounts
    void clear();

    /// Save wallet to file
    /// @param filepath The file path to save to
    /// @param password Optional password for encryption
    virtual void save(const std::string& filepath, const std::string& password = "") const;

    /// Load wallet from file
    /// @param filepath The file path to load from
    /// @param password Optional password for decryption
    /// @return The loaded wallet
    static SharedPtr<Wallet> load(const std::string& filepath, const std::string& password = "");

protected:
    /// Update internal indices after adding/removing accounts
    void updateIndices();
};

} // namespace neocpp
