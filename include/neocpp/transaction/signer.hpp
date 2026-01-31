#pragma once

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/transaction/witness_scope.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class WitnessRule;
class BinaryWriter;
class BinaryReader;

/// Represents a transaction signer
class Signer : public NeoSerializable {
protected:
    Hash160 account_;
    WitnessScope scopes_;
    std::vector<Hash160> allowedContracts_;
    std::vector<Bytes> allowedGroups_;
    std::vector<SharedPtr<WitnessRule>> rules_;

public:
    /// Constructor
    /// @param account The signer account script hash
    /// @param scopes The witness scopes
    Signer(const Hash160& account, WitnessScope scopes = WitnessScope::CALLED_BY_ENTRY);

    /// Destructor
    virtual ~Signer() = default;

    // Getters
    const Hash160& getAccount() const { return account_; }
    const Hash160& getScriptHash() const { return account_; }
    [[nodiscard]] WitnessScope getScopes() const { return scopes_; }
    const std::vector<Hash160>& getAllowedContracts() const { return allowedContracts_; }
    const std::vector<Bytes>& getAllowedGroups() const { return allowedGroups_; }
    const std::vector<SharedPtr<WitnessRule>>& getRules() const { return rules_; }

    // Setters
    void setScopes(WitnessScope scopes) { scopes_ = scopes; }

    /// Add an allowed contract
    /// @param contractHash The contract script hash
    void addAllowedContract(const Hash160& contractHash);

    /// Add an allowed group
    /// @param groupPubKey The group public key
    void addAllowedGroup(const Bytes& groupPubKey);

    /// Add a witness rule
    /// @param rule The witness rule
    void addRule(const SharedPtr<WitnessRule>& rule);

    /// Check if global scope
    /// @return True if global scope
    bool isGlobal() const;

    /// Check if has specific scope
    /// @param scope The scope to check
    /// @return True if has the scope
    bool hasScope(WitnessScope scope) const;

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static SharedPtr<Signer> deserialize(BinaryReader& reader);

    // Comparison operators
    bool operator==(const Signer& other) const;
    bool operator!=(const Signer& other) const;

    /// Convert to JSON representation
    nlohmann::json toJson() const;
};

} // namespace neocpp
