#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>
#include <vector>

namespace neocpp {

/// Policy native contract
class PolicyContract : public SmartContract {
public:
    static const Hash160 SCRIPT_HASH;
    static const std::string NAME;

    /// Constructor
    explicit PolicyContract(const SharedPtr<NeoRpcClient>& client);

    /// Create instance
    static SharedPtr<PolicyContract> create(const SharedPtr<NeoRpcClient>& client);

    /// Get fee per byte
    /// @return The fee per byte
    [[nodiscard]] int64_t getFeePerByte();

    /// Get execution fee factor
    /// @return The execution fee factor
    [[nodiscard]] int32_t getExecFeeFactor();

    /// Get storage price
    /// @return The storage price
    [[nodiscard]] int64_t getStoragePrice();

    /// Get maximum transactions per block
    /// @return The maximum transactions per block
    [[nodiscard]] uint32_t getMaxTransactionsPerBlock();

    /// Get maximum block size
    /// @return The maximum block size
    [[nodiscard]] uint32_t getMaxBlockSize();

    /// Get maximum block system fee
    /// @return The maximum block system fee
    [[nodiscard]] int64_t getMaxBlockSystemFee();

    /// Check if account is blocked
    /// @param account The account hash
    /// @return True if blocked
    bool isBlocked(const Hash160& account);

    /// Set fee per byte
    /// @param value The new fee per byte
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> setFeePerByte(int64_t value);

    /// Set execution fee factor
    /// @param value The new execution fee factor
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> setExecFeeFactor(int32_t value);

    /// Set storage price
    /// @param value The new storage price
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> setStoragePrice(int64_t value);

    /// Block account
    /// @param account The account to block
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> blockAccount(const Hash160& account);

    /// Unblock account
    /// @param account The account to unblock
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> unblockAccount(const Hash160& account);
};

} // namespace neocpp
