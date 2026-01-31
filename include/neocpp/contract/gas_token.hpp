#pragma once

#include "neocpp/contract/fungible_token.hpp"

namespace neocpp {

/// GAS token contract interface
class GasToken : public FungibleToken {
public:
    /// GAS token script hash on MainNet
    static const Hash160 SCRIPT_HASH;

    /// Constructor
    /// @param client The RPC client
    explicit GasToken(const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    ~GasToken() = default;

    /// Claim GAS rewards
    /// @param account The account to claim for
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> claim(const SharedPtr<Account>& account);

    /// Get the network fee per byte
    /// @return The fee per byte in GAS fractions
    [[nodiscard]] int64_t getFeePerByte();

    /// Get the execution fee factor
    /// @return The execution fee factor
    [[nodiscard]] int32_t getExecFeeFactor();

    /// Get the storage price
    /// @return The storage price per byte
    [[nodiscard]] int64_t getStoragePrice();
};

} // namespace neocpp
