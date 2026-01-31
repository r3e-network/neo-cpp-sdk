#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>
#include <vector>

namespace neocpp {

/// Base class for NEP-11 non-fungible token contracts
class NonFungibleToken : public SmartContract {
protected:
    std::string symbol_;
    int decimals_;
    mutable bool metadataLoaded_;

public:
    /// Constructor
    /// @param scriptHash The NFT contract script hash
    /// @param client The RPC client
    NonFungibleToken(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    virtual ~NonFungibleToken() = default;

    /// Get token symbol
    /// @return The token symbol
    std::string getSymbol();

    /// Get token decimals (usually 0 for NFTs)
    /// @return The number of decimals
    [[nodiscard]] int getDecimals();

    /// Get total supply
    /// @return The total supply
    [[nodiscard]] int64_t getTotalSupply();

    /// Get balance of an address
    /// @param address The address
    /// @return The balance (number of NFTs owned)
    [[nodiscard]] int64_t getBalanceOf(const std::string& address);

    /// Get tokens owned by an address
    /// @param address The address
    /// @return List of token IDs
    std::vector<std::string> getTokensOf(const std::string& address);

    /// Get owner of a token
    /// @param tokenId The token ID
    /// @return The owner address
    std::string getOwnerOf(const std::string& tokenId);

    /// Get token properties/metadata
    /// @param tokenId The token ID
    /// @return The token properties
    nlohmann::json getProperties(const std::string& tokenId);

    /// Get all tokens
    /// @return List of all token IDs
    std::vector<std::string> getAllTokens();

    /// Transfer NFT
    /// @param from The sender account
    /// @param to The recipient address
    /// @param tokenId The token ID to transfer
    /// @param data Optional data
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> transfer(const SharedPtr<Account>& from,
                                          const std::string& to,
                                          const std::string& tokenId,
                                          const std::string& data = "");

    /// Check if divisible (NEP-11 divisible standard)
    /// @return True if divisible NFT
    bool isDivisible();

    /// For divisible NFTs: Get balance of specific token for an address
    /// @param address The address
    /// @param tokenId The token ID
    /// @return The balance amount
    [[nodiscard]] int64_t getBalanceOf(const std::string& address, const std::string& tokenId);

    /// For divisible NFTs: Transfer amount of specific token
    /// @param from The sender account
    /// @param to The recipient address
    /// @param amount The amount to transfer
    /// @param tokenId The token ID
    /// @param data Optional data
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> transfer(const SharedPtr<Account>& from,
                                          const std::string& to,
                                          int64_t amount,
                                          const std::string& tokenId,
                                          const std::string& data = "");

protected:
    /// Load token metadata (symbol and decimals)
    void loadMetadata();
};

} // namespace neocpp
