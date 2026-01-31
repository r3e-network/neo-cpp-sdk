#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>

namespace neocpp {

/// Base class for NEP-17 fungible token contracts
class FungibleToken : public SmartContract {
protected:
    std::string symbol_;
    int decimals_;
    mutable bool metadataLoaded_;

public:
    /// Constructor
    /// @param scriptHash The token contract script hash
    /// @param client The RPC client
    FungibleToken(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    virtual ~FungibleToken() = default;

    /// Get token symbol
    /// @return The token symbol
    [[nodiscard]] std::string getSymbol();

    /// Get token decimals
    /// @return The number of decimals
    [[nodiscard]] int getDecimals();

    /// Get total supply
    /// @return The total supply
    [[nodiscard]] int64_t getTotalSupply();

    /// Get balance of an address
    /// @param address The address
    /// @return The balance
    [[nodiscard]] int64_t getBalanceOf(const std::string& address);

    /// Transfer tokens
    /// @param from The sender account
    /// @param to The recipient address
    /// @param amount The amount to transfer
    /// @param data Optional data
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> transfer(const SharedPtr<Account>& from,
                                          const std::string& to,
                                          int64_t amount,
                                          const std::string& data = "");

    /// Transfer tokens with multiple recipients
    /// @param from The sender account
    /// @param recipients List of (address, amount) pairs
    /// @param data Optional data
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> transferMultiple(const SharedPtr<Account>& from,
                                                  const std::vector<std::pair<std::string, int64_t>>& recipients,
                                                  const std::string& data = "");

    /// Convert amount to smallest unit considering decimals
    /// @param amount The amount in whole units
    /// @return The amount in smallest units
    int64_t toSmallestUnit(double amount);

    /// Convert amount from smallest unit considering decimals
    /// @param amount The amount in smallest units
    /// @return The amount in whole units
    double fromSmallestUnit(int64_t amount);

protected:
    /// Load token metadata (symbol and decimals)
    void loadMetadata();
};

} // namespace neocpp
