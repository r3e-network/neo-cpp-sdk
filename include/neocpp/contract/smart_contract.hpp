#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

// Forward declarations
class NeoRpcClient;
class TransactionBuilder;
class ContractParameter;
class Account;

/// Base class for smart contract interactions
class SmartContract {
protected:
    Hash160 scriptHash_;
    SharedPtr<NeoRpcClient> client_;

public:
    /// Constructor
    /// @param scriptHash The contract script hash
    /// @param client The RPC client
    SmartContract(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    virtual ~SmartContract() = default;

    /// Get the contract script hash
    const Hash160& getScriptHash() const { return scriptHash_; }

    /// Get the RPC client
    const SharedPtr<NeoRpcClient>& getClient() const { return client_; }

    /// Set the RPC client
    void setClient(const SharedPtr<NeoRpcClient>& client) { client_ = client; }

    /// Invoke a contract method (read-only)
    /// @param method The method name
    /// @param params The parameters
    /// @return The invocation result
    nlohmann::json invokeFunction(const std::string& method, const std::vector<ContractParameter>& params = {});

    /// Build invocation transaction
    /// @param method The method name
    /// @param params The parameters
    /// @param account The account to sign with
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> buildInvokeTx(const std::string& method,
                                                const std::vector<ContractParameter>& params,
                                                const SharedPtr<Account>& account);

    /// Get contract manifest
    /// @return The contract manifest
    [[nodiscard]] nlohmann::json getManifest();

    /// Get contract NEF
    /// @return The contract NEF
    [[nodiscard]] std::string getNef();

    /// Check if contract is deployed
    /// @return True if deployed
    [[nodiscard]] bool isDeployed();

    /// Get contract name
    /// @return The contract name from manifest
    [[nodiscard]] std::string getName();

    /// Get supported standards
    /// @return List of supported standards
    [[nodiscard]] std::vector<std::string> getSupportedStandards();

    /// Get ABI
    /// @return The contract ABI
    [[nodiscard]] nlohmann::json getAbi();

    /// Get contract methods
    /// @return List of contract methods
    [[nodiscard]] std::vector<nlohmann::json> getMethods();

    /// Get contract events
    /// @return List of contract events
    [[nodiscard]] std::vector<nlohmann::json> getEvents();

protected:
    /// Convert parameters to JSON for RPC
    [[nodiscard]] nlohmann::json paramsToJson(const std::vector<ContractParameter>& params);
};

} // namespace neocpp
