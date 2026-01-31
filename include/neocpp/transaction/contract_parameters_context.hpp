#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

// Forward declarations
class Transaction;
class Account;
class Witness;

/// Context for collecting signatures for multi-sig transactions
class ContractParametersContext {
private:
    SharedPtr<Transaction> transaction_;
    std::map<Hash160, std::vector<Bytes>> signatures_;
    std::map<Hash160, Bytes> verificationScripts_;
    mutable std::map<Hash160, std::pair<int, int>> scriptInfo_; // m,n values for multi-sig

public:
    /// Constructor
    /// @param transaction The transaction to sign
    explicit ContractParametersContext(const SharedPtr<Transaction>& transaction);

    /// Destructor
    ~ContractParametersContext() = default;

    /// Get the transaction
    SharedPtr<Transaction> getTransaction() const { return transaction_; }

    /// Add signature
    /// @param account The signing account
    /// @param signature The signature
    void addSignature(const SharedPtr<Account>& account, const Bytes& signature);

    /// Add signature
    /// @param scriptHash The script hash
    /// @param publicKey The public key
    /// @param signature The signature
    void addSignature(const Hash160& scriptHash, const Bytes& publicKey, const Bytes& signature);

    /// Sign with account
    /// @param account The account to sign with
    /// @return True if signature was added
    bool sign(const SharedPtr<Account>& account);

    /// Check if all signatures are collected
    /// @return True if complete
    bool isComplete() const;

    /// Check if script hash is complete
    /// @param scriptHash The script hash
    /// @return True if complete
    bool isComplete(const Hash160& scriptHash) const;

    /// Get witnesses
    /// @return The witnesses
    std::vector<SharedPtr<Witness>> getWitnesses() const;

    /// Get witness for script hash
    /// @param scriptHash The script hash
    /// @return The witness
    SharedPtr<Witness> getWitness(const Hash160& scriptHash) const;

    /// Export to JSON
    /// @return JSON representation
    nlohmann::json toJson() const;

    /// Import from JSON
    /// @param json The JSON data
    /// @return The context
    static SharedPtr<ContractParametersContext> fromJson(const nlohmann::json& json);

private:
    /// Parse verification script
    void parseVerificationScript(const Hash160& scriptHash, const Bytes& script);

    /// Get required signature count
    [[nodiscard]] int getRequiredSignatures(const Hash160& scriptHash) const;

    /// Get collected signature count
    [[nodiscard]] int getCollectedSignatures(const Hash160& scriptHash) const;
};

} // namespace neocpp
