#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>
#include <vector>

namespace neocpp {

// Forward declarations
class NefFile;
class ContractManifest;

/// Contract management native contract
class ContractManagement : public SmartContract {
public:
    static const Hash160 SCRIPT_HASH;
    static const std::string NAME;

    /// Constructor
    explicit ContractManagement(const SharedPtr<NeoRpcClient>& client);

    /// Create instance
    static SharedPtr<ContractManagement> create(const SharedPtr<NeoRpcClient>& client);

    /// Deploy a contract
    /// @param nef The NEF file
    /// @param manifest The contract manifest
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> deploy(const SharedPtr<NefFile>& nef,
                                         const SharedPtr<ContractManifest>& manifest);

    /// Update a contract
    /// @param scriptHash The contract to update
    /// @param nef The new NEF file (optional)
    /// @param manifest The new manifest (optional)
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> update(const Hash160& scriptHash,
                                         const SharedPtr<NefFile>& nef = nullptr,
                                         const SharedPtr<ContractManifest>& manifest = nullptr);

    /// Destroy a contract
    /// @param scriptHash The contract to destroy
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> destroy(const Hash160& scriptHash);

    /// Get contract by hash
    /// @param scriptHash The contract hash
    /// @return Contract state
    nlohmann::json getContract(const Hash160& scriptHash);

    /// Check if contract has method
    /// @param scriptHash The contract hash
    /// @param method The method name
    /// @param paramCount The parameter count
    /// @return True if method exists
    bool hasMethod(const Hash160& scriptHash, const std::string& method, int paramCount);

    /// Get minimum deployment fee
    /// @return The minimum fee
    [[nodiscard]] int64_t getMinimumDeploymentFee();
};

} // namespace neocpp
