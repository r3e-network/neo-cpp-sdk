#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash256.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

// Forward declarations
class Transaction;
class Block;
class HttpService;
class NeoGetVersionResponse;
class NeoGetBlockResponse;
class NeoGetRawTransactionResponse;
class NeoGetApplicationLogResponse;
class NeoGetContractStateResponse;
class NeoGetNep17BalancesResponse;
class NeoInvokeResultResponse;
class NeoGetPeersResponse;
class NeoGetUnclaimedGasResponse;
class NeoGetWalletBalanceResponse;

/// Neo RPC client for interacting with Neo nodes
class NeoRpcClient {
private:
    std::string url_;
    SharedPtr<HttpService> httpService_;
    int requestId_;

public:
    /// Constructor
    /// @param url The RPC endpoint URL
    explicit NeoRpcClient(const std::string& url);

    /// Destructor
    ~NeoRpcClient() = default;

    /// Get the RPC URL
    const std::string& getUrl() const { return url_; }

    /// Set the RPC URL
    void setUrl(const std::string& url) { url_ = url; }

    // Node methods

    /// Get node version information
    /// @return Version information
    SharedPtr<NeoGetVersionResponse> getVersion();

    /// Get connection count
    /// @return Number of connections
    [[nodiscard]] int getConnectionCount();

    /// Get list of peers
    /// @return Peer information
    SharedPtr<NeoGetPeersResponse> getPeers();

    /// Validate an address
    /// @param address The address to validate
    /// @return Validation result
    nlohmann::json validateAddress(const std::string& address);

    // Blockchain methods

    /// Get best block hash
    /// @return The best block hash
    [[nodiscard]] Hash256 getBestBlockHash();

    /// Get block by hash
    /// @param hash The block hash
    /// @param verbose Whether to return verbose data
    /// @return Block information
    SharedPtr<NeoGetBlockResponse> getBlock(const Hash256& hash, bool verbose = true);

    /// Get block by index
    /// @param index The block index
    /// @param verbose Whether to return verbose data
    /// @return Block information
    SharedPtr<NeoGetBlockResponse> getBlock(uint32_t index, bool verbose = true);

    /// Get block count
    /// @return The current block count
    [[nodiscard]] uint32_t getBlockCount();

    /// Get block hash by index
    /// @param index The block index
    /// @return The block hash
    [[nodiscard]] Hash256 getBlockHash(uint32_t index);

    /// Get block header by hash
    /// @param hash The block hash
    /// @param verbose Whether to return verbose data
    /// @return Block header information
    nlohmann::json getBlockHeader(const Hash256& hash, bool verbose = true);

    /// Get block header by index
    /// @param index The block index
    /// @param verbose Whether to return verbose data
    /// @return Block header information
    nlohmann::json getBlockHeader(uint32_t index, bool verbose = true);

    /// Get raw transaction
    /// @param txId The transaction ID
    /// @param verbose Whether to return verbose data
    /// @return Transaction information
    SharedPtr<NeoGetRawTransactionResponse> getRawTransaction(const Hash256& txId, bool verbose = true);

    /// Get transaction height
    /// @param txId The transaction ID
    /// @return The block height containing the transaction
    [[nodiscard]] uint32_t getTransactionHeight(const Hash256& txId);

    // Smart contract methods

    /// Get contract state
    /// @param scriptHash The contract script hash
    /// @return Contract state information
    SharedPtr<NeoGetContractStateResponse> getContractState(const Hash160& scriptHash);

    /// Get NEP-17 balances
    /// @param address The address to query
    /// @return NEP-17 token balances
    SharedPtr<NeoGetNep17BalancesResponse> getNep17Balances(const std::string& address);

    /// Get NEP-17 transfers
    /// @param address The address to query
    /// @param startTime Optional start timestamp
    /// @param endTime Optional end timestamp
    /// @return Transfer history
    nlohmann::json getNep17Transfers(const std::string& address, uint64_t startTime = 0, uint64_t endTime = 0);

    /// Get storage value
    /// @param scriptHash The contract script hash
    /// @param key The storage key
    /// @return The storage value
    std::string getStorage(const Hash160& scriptHash, const std::string& key);

    /// Find storage values
    /// @param scriptHash The contract script hash
    /// @param prefix The key prefix
    /// @return Storage entries
    nlohmann::json findStorage(const Hash160& scriptHash, const std::string& prefix);

    // Invocation methods

    /// Invoke contract function (read-only)
    /// @param scriptHash The contract script hash
    /// @param method The method name
    /// @param params The parameters
    /// @param signers Optional signers
    /// @return Invocation result
    SharedPtr<NeoInvokeResultResponse> invokeFunction(const Hash160& scriptHash,
                                                      const std::string& method,
                                                      const nlohmann::json& params = nlohmann::json::array(),
                                                      const nlohmann::json& signers = nlohmann::json::array());

    /// Invoke script (read-only)
    /// @param script The script to invoke
    /// @param signers Optional signers
    /// @return Invocation result
    SharedPtr<NeoInvokeResultResponse> invokeScript(const Bytes& script,
                                                    const nlohmann::json& signers = nlohmann::json::array());

    /// Invoke script (read-only) with base64 encoding
    /// @param script The base64-encoded script
    /// @param signers Optional signers
    /// @return Invocation result
    SharedPtr<NeoInvokeResultResponse> invokeScript(const std::string& base64Script,
                                                    const nlohmann::json& signers = nlohmann::json::array());

    // Transaction methods

    /// Send raw transaction
    /// @param transaction The transaction to send
    /// @return The transaction hash
    Hash256 sendRawTransaction(const SharedPtr<Transaction>& transaction);

    /// Send raw transaction from hex
    /// @param hex The hex-encoded transaction
    /// @return The transaction hash
    Hash256 sendRawTransaction(const std::string& hex);

    /// Calculate network fee for transaction
    /// @param transaction The transaction
    /// @return The network fee in GAS fractions
    int64_t calculateNetworkFee(const SharedPtr<Transaction>& transaction);

    /// Get application log for transaction
    /// @param txId The transaction ID
    /// @return Application log
    SharedPtr<NeoGetApplicationLogResponse> getApplicationLog(const Hash256& txId);

    // Wallet methods

    /// Get unclaimed GAS
    /// @param address The address
    /// @return Unclaimed GAS amount
    SharedPtr<NeoGetUnclaimedGasResponse> getUnclaimedGas(const std::string& address);

    /// Get wallet balance
    /// @param assetHash The asset hash
    /// @param address The address
    /// @return Balance information
    SharedPtr<NeoGetWalletBalanceResponse> getWalletBalance(const Hash160& assetHash, const std::string& address);

    // Committee methods

    /// Get committee members
    /// @return List of committee member public keys
    std::vector<std::string> getCommittee();

    /// Get next block validators
    /// @return List of validator public keys
    std::vector<nlohmann::json> getNextBlockValidators();

    // State methods

    /// Get state root by index
    /// @param index The state root index
    /// @return State root information
    nlohmann::json getStateRoot(uint32_t index);

    /// Get proof for key
    /// @param rootHash The state root hash
    /// @param contractHash The contract hash
    /// @param key The storage key
    /// @return Proof data
    nlohmann::json getProof(const Hash256& rootHash, const Hash160& contractHash, const std::string& key);

    /// Verify proof
    /// @param rootHash The state root hash
    /// @param proof The proof data
    /// @return Verification result
    bool verifyProof(const Hash256& rootHash, const std::string& proof);

    /// Get state height
    /// @return Current state height information
    nlohmann::json getStateHeight();

    // Iterator methods

    /// Traverse iterator
    /// @param sessionId The session ID
    /// @param iteratorId The iterator ID
    /// @param count The number of items to retrieve
    /// @return Iterator values
    nlohmann::json traverseIterator(const std::string& sessionId, const std::string& iteratorId, uint32_t count);

    /// Terminate session
    /// @param sessionId The session ID to terminate
    /// @return Success status
    bool terminateSession(const std::string& sessionId);

    // Raw JSON-RPC methods

    /// Send raw JSON-RPC request
    /// @param method The RPC method name
    /// @param params The parameters
    /// @return The response
    nlohmann::json sendRequest(const std::string& method, const nlohmann::json& params = nlohmann::json::array());

    /// Send batch of JSON-RPC requests
    /// @param requests The batch of requests
    /// @return The responses
    std::vector<nlohmann::json> sendBatch(const std::vector<std::pair<std::string, nlohmann::json>>& requests);

private:
    /// Generate next request ID
    [[nodiscard]] int getNextRequestId();

    /// Build JSON-RPC request
    nlohmann::json buildRequest(const std::string& method, const nlohmann::json& params);

    /// Parse JSON-RPC response
    nlohmann::json parseResponse(const std::string& response);

    /// Handle JSON-RPC error
    void handleError(const nlohmann::json& error);
};

} // namespace neocpp
