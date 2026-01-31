#pragma once

#include <string>
#include <memory>
#include <chrono>
#include "neocpp/types/hash160.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/protocol/http_service.hpp"

namespace neocpp {

/// Configuration for NeoCpp client
struct NeoCppConfig {
    /// Default block time in milliseconds (15 seconds on Neo N3)
    static constexpr int DEFAULT_BLOCK_TIME = 15000;

    /// Maximum valid until block increment base
    static constexpr int MAX_VALID_UNTIL_BLOCK_INCREMENT_BASE = 86400000;

    /// The NeoNameService resolver script hash
    Hash160 nnsResolver;

    /// The interval in milliseconds in which blocks are produced
    int blockInterval = DEFAULT_BLOCK_TIME;

    /// The interval in milliseconds in which NeoCpp should poll the Neo node for new block information
    int pollingInterval = DEFAULT_BLOCK_TIME;

    /// The maximum time in milliseconds that can pass from the construction of a transaction until it gets included in a block
    int maxValidUntilBlockIncrement = MAX_VALID_UNTIL_BLOCK_INCREMENT_BASE / DEFAULT_BLOCK_TIME;

    /// Whether to allow transmission of scripts that lead to a FAULT state
    bool allowTransmissionOnFault = false;

    /// Set to allow transmission on fault
    void setAllowTransmissionOnFault(bool allow) {
        allowTransmissionOnFault = allow;
    }
};

/// Main NeoCpp client class for interacting with Neo blockchain
class NeoCpp {
private:
    NeoCppConfig config_;
    SharedPtr<HttpService> httpService_;
    SharedPtr<NeoRpcClient> rpcClient_;

public:
    /// Constructor with configuration and HTTP service
    /// @param config The configuration to use
    /// @param httpService The HTTP service for network communication
    NeoCpp(const NeoCppConfig& config, SharedPtr<HttpService> httpService);

    /// Constructor with URL only (uses default configuration)
    /// @param url The RPC endpoint URL
    explicit NeoCpp(const std::string& url);

    /// Static factory method to build a NeoCpp instance
    /// @param httpService The HTTP service to use
    /// @param config The configuration (optional, defaults to new one)
    /// @return A new NeoCpp instance
    static SharedPtr<NeoCpp> build(SharedPtr<HttpService> httpService, const NeoCppConfig& config = NeoCppConfig());

    /// Static factory method to build a NeoCpp instance from URL
    /// @param url The RPC endpoint URL
    /// @param config The configuration (optional, defaults to new one)
    /// @return A new NeoCpp instance
    static SharedPtr<NeoCpp> build(const std::string& url, const NeoCppConfig& config = NeoCppConfig());

    // Configuration getters

    /// Get the configuration
    const NeoCppConfig& getConfig() const { return config_; }

    /// Get the NeoNameService resolver script hash
    const Hash160& getNnsResolver() const { return config_.nnsResolver; }

    /// Get the block interval in milliseconds
    [[nodiscard]] int getBlockInterval() const { return config_.blockInterval; }

    /// Get the polling interval in milliseconds
    [[nodiscard]] int getPollingInterval() const { return config_.pollingInterval; }

    /// Get the maximum valid until block increment
    [[nodiscard]] int getMaxValidUntilBlockIncrement() const { return config_.maxValidUntilBlockIncrement; }

    /// Check if transmission on fault is allowed
    bool isTransmissionOnFaultAllowed() const { return config_.allowTransmissionOnFault; }

    /// Allow transmission of scripts that lead to a FAULT state
    void allowTransmissionOnFault() {
        config_.setAllowTransmissionOnFault(true);
    }

    /// Prevent transmission of scripts that lead to a FAULT state
    void preventTransmissionOnFault() {
        config_.setAllowTransmissionOnFault(false);
    }

    // RPC Client access

    /// Get the underlying RPC client
    /// @return The RPC client
    SharedPtr<NeoRpcClient> getRpcClient() { return rpcClient_; }

    /// Get the underlying RPC client (const version)
    /// @return The RPC client
    SharedPtr<const NeoRpcClient> getRpcClient() const { return rpcClient_; }

    // Convenience methods that delegate to RPC client

    /// Get the current block count
    /// @return The block count
    [[nodiscard]] uint32_t getBlockCount();

    /// Get the best block hash
    /// @return The best block hash
    [[nodiscard]] Hash256 getBestBlockHash();

    /// Get a block by its hash
    /// @param hash The block hash
    /// @param verbose Whether to return verbose data
    /// @return The block information
    SharedPtr<NeoGetBlockResponse> getBlock(const Hash256& hash, bool verbose = true);

    /// Get a block by its index
    /// @param index The block index
    /// @param verbose Whether to return verbose data
    /// @return The block information
    SharedPtr<NeoGetBlockResponse> getBlock(uint32_t index, bool verbose = true);

    /// Get transaction by its hash
    /// @param txId The transaction ID
    /// @param verbose Whether to return verbose data
    /// @return The transaction information
    SharedPtr<NeoGetRawTransactionResponse> getTransaction(const Hash256& txId, bool verbose = true);

    /// Get contract state
    /// @param scriptHash The contract script hash
    /// @return The contract state
    SharedPtr<NeoGetContractStateResponse> getContractState(const Hash160& scriptHash);

    /// Get NEP-17 balances for an address
    /// @param address The address to query
    /// @return The NEP-17 balances
    SharedPtr<NeoGetNep17BalancesResponse> getNep17Balances(const std::string& address);

    /// Invoke a contract function (read-only)
    /// @param scriptHash The contract script hash
    /// @param method The method name
    /// @param params The parameters
    /// @param signers The signers (optional)
    /// @return The invocation result
    SharedPtr<NeoInvokeResultResponse> invokeFunction(const Hash160& scriptHash,
                                                      const std::string& method,
                                                      const nlohmann::json& params = nlohmann::json::array(),
                                                      const nlohmann::json& signers = nlohmann::json::array());

    /// Send a raw transaction
    /// @param transaction The transaction to send
    /// @return The transaction hash
    Hash256 sendRawTransaction(const SharedPtr<Transaction>& transaction);

    /// Validate an address
    /// @param address The address to validate
    /// @return Validation result
    nlohmann::json validateAddress(const std::string& address);

    /// Get the version information
    /// @return Version information
    SharedPtr<NeoGetVersionResponse> getVersion();
};

} // namespace neocpp
