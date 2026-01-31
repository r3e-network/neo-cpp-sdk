#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/hash256.hpp"

namespace neocpp {

// Forward declarations
class NeoRpcClient;
class HttpService;
class BlockPolling;
class Signer;
class ContractParameter;
class TransactionSendToken;
class NeoBlock;
class ContractState;
class NativeContractState;
class Transaction;
class InvocationResult;
class NeoApplicationLog;
class MemPoolDetails;
class NeoVersion;
class Peers;
class Validator;
class GetUnclaimedGas;
class NeoNetworkFee;
class NeoAddress;
class RawTransaction;
class Plugin;
class Balance;
class Nep17Balances;
class Nep17Transfers;
class Nep11Balances;
class Nep11Transfers;
class StateRoot;
class StateHeight;
class States;
class StackItem;

/// Main Neo blockchain interface
class Neo {
protected:
    SharedPtr<NeoRpcClient> rpcClient_;
    SharedPtr<HttpService> httpService_;
    SharedPtr<BlockPolling> blockPolling_;
    std::string rpcUrl_;

public:
    /// Constructor with default configuration
    Neo();

    /// Constructor with RPC URL
    /// @param rpcUrl The RPC endpoint URL
    explicit Neo(const std::string& rpcUrl);

    /// Destructor
    virtual ~Neo() = default;

    /// Get RPC client
    SharedPtr<NeoRpcClient> getRpcClient() const { return rpcClient_; }

    /// Get HTTP service
    SharedPtr<HttpService> getHttpService() const { return httpService_; }

    /// Get RPC URL
    const std::string& getRpcUrl() const { return rpcUrl_; }

    /// Subscribe to new blocks
    /// @param callback The callback function
    void subscribeToBlocks(std::function<void(uint32_t)> callback);

    /// Unsubscribe from blocks
    void unsubscribeFromBlocks();

    /// Check if connected
    /// @return True if connected to node
    bool isConnected() const;

    /// Get network magic
    /// @return The network magic number
    uint32_t getNetworkMagic() const;

    /// Get node version
    /// @return The node version info
    std::string getNodeVersion() const;

    /// Shutdown and cleanup
    void shutdown();

    // ========================================
    // BLOCKCHAIN METHODS
    // ========================================

    /// Get the hash of the best (latest) block
    /// @return The hash of the best block
    Hash256 getBestBlockHash();

    /// Get the hash of a block at a specific index
    /// @param blockIndex The block index
    /// @return The hash of the block
    Hash256 getBlockHash(int blockIndex);

    /// Get block information
    /// @param blockHash The block hash
    /// @param returnFullTransactionObjects Whether to return full transaction objects
    /// @return The block information
    NeoBlock getBlock(const Hash256& blockHash, bool returnFullTransactionObjects = true);

    /// Get block information by index
    /// @param blockIndex The block index
    /// @param returnFullTransactionObjects Whether to return full transaction objects
    /// @return The block information
    NeoBlock getBlock(int blockIndex, bool returnFullTransactionObjects = true);

    /// Get the raw block data as hex string
    /// @param blockHash The block hash
    /// @return The raw block data
    std::string getRawBlock(const Hash256& blockHash);

    /// Get the raw block data as hex string by index
    /// @param blockIndex The block index
    /// @return The raw block data
    std::string getRawBlock(int blockIndex);

    /// Get the current block count
    /// @return The number of blocks in the blockchain
    int getBlockCount();

    /// Get the block header count
    /// @return The number of block headers
    int getBlockHeaderCount();

    /// Get block header information
    /// @param blockHash The block hash
    /// @return The block header information
    NeoBlock getBlockHeader(const Hash256& blockHash);

    /// Get block header information by index
    /// @param blockIndex The block index
    /// @return The block header information
    NeoBlock getBlockHeader(int blockIndex);

    /// Get the raw block header data as hex string
    /// @param blockHash The block hash
    /// @return The raw block header data
    std::string getRawBlockHeader(const Hash256& blockHash);

    /// Get the raw block header data as hex string by index
    /// @param blockIndex The block index
    /// @return The raw block header data
    std::string getRawBlockHeader(int blockIndex);

    /// Get information about native contracts
    /// @return List of native contracts
    std::vector<NativeContractState> getNativeContracts();

    /// Get the state of a contract
    /// @param contractHash The contract hash
    /// @return The contract state
    ContractState getContractState(const Hash160& contractHash);

    /// Get the state of a native contract by name
    /// @param contractName The native contract name
    /// @return The contract state
    ContractState getNativeContractState(const std::string& contractName);

    /// Get memory pool details
    /// @return The memory pool details
    MemPoolDetails getMemPool();

    /// Get raw memory pool transaction hashes
    /// @return List of transaction hashes in memory pool
    std::vector<Hash256> getRawMemPool();

    /// Get transaction information
    /// @param txHash The transaction hash
    /// @return The transaction information
    Transaction getTransaction(const Hash256& txHash);

    /// Get raw transaction data as hex string
    /// @param txHash The transaction hash
    /// @return The raw transaction data
    std::string getRawTransaction(const Hash256& txHash);

    /// Get storage value from a contract
    /// @param contractHash The contract hash
    /// @param keyHexString The storage key as hex string
    /// @return The storage value
    std::string getStorage(const Hash160& contractHash, const std::string& keyHexString);

    /// Get the height of a transaction
    /// @param txHash The transaction hash
    /// @return The block height of the transaction
    int getTransactionHeight(const Hash256& txHash);

    /// Get next block validators
    /// @return List of validators for the next block
    std::vector<Validator> getNextBlockValidators();

    /// Get committee members
    /// @return List of committee member public keys
    std::vector<std::string> getCommittee();

    // ========================================
    // NODE METHODS
    // ========================================

    /// Get the connection count
    /// @return The number of connections
    int getConnectionCount();

    /// Get peer information
    /// @return The peer information
    Peers getPeers();

    /// Get version information
    /// @return The version information
    NeoVersion getVersion();

    /// Send a raw transaction
    /// @param rawTransactionHex The raw transaction as hex string
    /// @return The transaction result
    RawTransaction sendRawTransaction(const std::string& rawTransactionHex);

    /// Submit a new block
    /// @param serializedBlockAsHex The serialized block as hex string
    /// @return True if successful
    bool submitBlock(const std::string& serializedBlockAsHex);

    // ========================================
    // SMART CONTRACT METHODS
    // ========================================

    /// Invoke a contract function
    /// @param contractHash The contract hash
    /// @param functionName The function name
    /// @param signers The signers
    /// @return The invocation result
    InvocationResult invokeFunction(const Hash160& contractHash, const std::string& functionName,
                                   const std::vector<Signer>& signers = {});

    /// Invoke a contract function with parameters
    /// @param contractHash The contract hash
    /// @param functionName The function name
    /// @param params The contract parameters
    /// @param signers The signers
    /// @return The invocation result
    InvocationResult invokeFunction(const Hash160& contractHash, const std::string& functionName,
                                   const std::vector<ContractParameter>& params,
                                   const std::vector<Signer>& signers = {});

    /// Invoke a contract function with diagnostics
    /// @param contractHash The contract hash
    /// @param functionName The function name
    /// @param signers The signers
    /// @return The invocation result with diagnostics
    InvocationResult invokeFunctionDiagnostics(const Hash160& contractHash, const std::string& functionName,
                                              const std::vector<Signer>& signers = {});

    /// Invoke a contract function with parameters and diagnostics
    /// @param contractHash The contract hash
    /// @param functionName The function name
    /// @param params The contract parameters
    /// @param signers The signers
    /// @return The invocation result with diagnostics
    InvocationResult invokeFunctionDiagnostics(const Hash160& contractHash, const std::string& functionName,
                                              const std::vector<ContractParameter>& params,
                                              const std::vector<Signer>& signers = {});

    /// Invoke a script
    /// @param scriptHex The script as hex string
    /// @param signers The signers
    /// @return The invocation result
    InvocationResult invokeScript(const std::string& scriptHex, const std::vector<Signer>& signers = {});

    /// Invoke a script with diagnostics
    /// @param scriptHex The script as hex string
    /// @param signers The signers
    /// @return The invocation result with diagnostics
    InvocationResult invokeScriptDiagnostics(const std::string& scriptHex, const std::vector<Signer>& signers = {});

    /// Traverse an iterator
    /// @param sessionId The session ID
    /// @param iteratorId The iterator ID
    /// @param count The number of items to retrieve
    /// @return The stack items
    std::vector<StackItem> traverseIterator(const std::string& sessionId, const std::string& iteratorId, int count);

    /// Terminate a session
    /// @param sessionId The session ID
    /// @return True if successful
    bool terminateSession(const std::string& sessionId);

    /// Invoke contract verification
    /// @param contractHash The contract hash
    /// @param methodParameters The method parameters
    /// @param signers The signers
    /// @return The invocation result
    InvocationResult invokeContractVerify(const Hash160& contractHash,
                                         const std::vector<ContractParameter>& methodParameters,
                                         const std::vector<Signer>& signers = {});

    /// Get unclaimed gas
    /// @param scriptHash The script hash
    /// @return The unclaimed gas information
    GetUnclaimedGas getUnclaimedGas(const Hash160& scriptHash);

    // ========================================
    // UTILITIES METHODS
    // ========================================

    /// List available plugins
    /// @return List of plugins
    std::vector<Plugin> listPlugins();

    /// Validate an address
    /// @param address The address to validate
    /// @return The validation result
    bool validateAddress(const std::string& address);

    // ========================================
    // WALLET METHODS
    // ========================================

    /// Close the wallet
    /// @return True if successful
    bool closeWallet();

    /// Dump private key
    /// @param scriptHash The script hash
    /// @return The private key in WIF format
    std::string dumpPrivKey(const Hash160& scriptHash);

    /// Get wallet balance for a token
    /// @param tokenHash The token hash
    /// @return The balance
    Balance getWalletBalance(const Hash160& tokenHash);

    /// Get a new address
    /// @return The new address
    std::string getNewAddress();

    /// Get wallet unclaimed gas
    /// @return The unclaimed gas amount
    std::string getWalletUnclaimedGas();

    /// Import a private key
    /// @param privateKeyInWIF The private key in WIF format
    /// @return The Neo address
    NeoAddress importPrivKey(const std::string& privateKeyInWIF);

    /// Calculate network fee for a transaction
    /// @param transactionHex The transaction as hex string
    /// @return The network fee
    NeoNetworkFee calculateNetworkFee(const std::string& transactionHex);

    /// List wallet addresses
    /// @return List of addresses
    std::vector<NeoAddress> listAddress();

    /// Open a wallet
    /// @param walletPath The wallet path
    /// @param password The wallet password
    /// @return True if successful
    bool openWallet(const std::string& walletPath, const std::string& password);

    /// Send tokens from a specific address
    /// @param tokenHash The token hash
    /// @param from The from address
    /// @param to The to address
    /// @param amount The amount
    /// @return The transaction
    Transaction sendFrom(const Hash160& tokenHash, const Hash160& from, const Hash160& to, int amount);

    /// Send tokens using send token structure
    /// @param from The from address
    /// @param txSendToken The send token structure
    /// @return The transaction
    Transaction sendFrom(const Hash160& from, const TransactionSendToken& txSendToken);

    /// Send many tokens
    /// @param txSendTokens List of send tokens
    /// @return The transaction
    Transaction sendMany(const std::vector<TransactionSendToken>& txSendTokens);

    /// Send many tokens from a specific address
    /// @param from The from address
    /// @param txSendTokens List of send tokens
    /// @return The transaction
    Transaction sendMany(const Hash160& from, const std::vector<TransactionSendToken>& txSendTokens);

    /// Send tokens to an address
    /// @param tokenHash The token hash
    /// @param to The to address
    /// @param amount The amount
    /// @return The transaction
    Transaction sendToAddress(const Hash160& tokenHash, const Hash160& to, int amount);

    /// Send tokens using send token structure
    /// @param txSendToken The send token structure
    /// @return The transaction
    Transaction sendToAddress(const TransactionSendToken& txSendToken);

    // ========================================
    // APPLICATION LOGS
    // ========================================

    /// Get application log for a transaction
    /// @param txHash The transaction hash
    /// @return The application log
    NeoApplicationLog getApplicationLog(const Hash256& txHash);

    // ========================================
    // TOKEN TRACKER NEP-17
    // ========================================

    /// Get NEP-17 token balances
    /// @param scriptHash The script hash
    /// @return The NEP-17 balances
    Nep17Balances getNep17Balances(const Hash160& scriptHash);

    /// Get NEP-17 token transfers
    /// @param scriptHash The script hash
    /// @return The NEP-17 transfers
    Nep17Transfers getNep17Transfers(const Hash160& scriptHash);

    /// Get NEP-17 token transfers from a date
    /// @param scriptHash The script hash
    /// @param from The from date
    /// @return The NEP-17 transfers
    Nep17Transfers getNep17Transfers(const Hash160& scriptHash, const std::chrono::system_clock::time_point& from);

    /// Get NEP-17 token transfers between dates
    /// @param scriptHash The script hash
    /// @param from The from date
    /// @param to The to date
    /// @return The NEP-17 transfers
    Nep17Transfers getNep17Transfers(const Hash160& scriptHash,
                                    const std::chrono::system_clock::time_point& from,
                                    const std::chrono::system_clock::time_point& to);

    // ========================================
    // TOKEN TRACKER NEP-11
    // ========================================

    /// Get NEP-11 token balances
    /// @param scriptHash The script hash
    /// @return The NEP-11 balances
    Nep11Balances getNep11Balances(const Hash160& scriptHash);

    /// Get NEP-11 token transfers
    /// @param scriptHash The script hash
    /// @return The NEP-11 transfers
    Nep11Transfers getNep11Transfers(const Hash160& scriptHash);

    /// Get NEP-11 token transfers from a date
    /// @param scriptHash The script hash
    /// @param from The from date
    /// @return The NEP-11 transfers
    Nep11Transfers getNep11Transfers(const Hash160& scriptHash, const std::chrono::system_clock::time_point& from);

    /// Get NEP-11 token transfers between dates
    /// @param scriptHash The script hash
    /// @param from The from date
    /// @param to The to date
    /// @return The NEP-11 transfers
    Nep11Transfers getNep11Transfers(const Hash160& scriptHash,
                                    const std::chrono::system_clock::time_point& from,
                                    const std::chrono::system_clock::time_point& to);

    /// Get NEP-11 token properties
    /// @param scriptHash The script hash
    /// @param tokenId The token ID
    /// @return The token properties
    std::map<std::string, std::string> getNep11Properties(const Hash160& scriptHash, const std::string& tokenId);

    // ========================================
    // STATE SERVICE
    // ========================================

    /// Get state root
    /// @param blockIndex The block index
    /// @return The state root
    StateRoot getStateRoot(int blockIndex);

    /// Get proof data
    /// @param rootHash The root hash
    /// @param contractHash The contract hash
    /// @param storageKeyHex The storage key as hex string
    /// @return The proof data
    std::string getProof(const Hash256& rootHash, const Hash160& contractHash, const std::string& storageKeyHex);

    /// Verify proof data
    /// @param rootHash The root hash
    /// @param proofDataHex The proof data as hex string
    /// @return The verification result
    std::string verifyProof(const Hash256& rootHash, const std::string& proofDataHex);

    /// Get state height
    /// @return The state height
    StateHeight getStateHeight();

    /// Get state
    /// @param rootHash The root hash
    /// @param contractHash The contract hash
    /// @param keyHex The key as hex string
    /// @return The state value
    std::string getState(const Hash256& rootHash, const Hash160& contractHash, const std::string& keyHex);

    /// Find states
    /// @param rootHash The root hash
    /// @param contractHash The contract hash
    /// @param storageKeyPrefix The storage key prefix
    /// @param startKey The start key (optional)
    /// @param count The count (optional)
    /// @return The states
    States findStates(const Hash256& rootHash, const Hash160& contractHash, const std::string& storageKeyPrefix,
                     const std::optional<std::string>& startKey = std::nullopt,
                     const std::optional<int>& count = std::nullopt);

protected:
    /// Initialize components
    void initialize();
};

} // namespace neocpp
