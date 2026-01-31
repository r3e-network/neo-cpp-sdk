#pragma once

#include "neocpp/contract/fungible_token.hpp"

namespace neocpp {

/// NEO token contract interface
class NeoToken : public FungibleToken {
public:
    /// NEO token script hash on MainNet
    static const Hash160 SCRIPT_HASH;

    /// Constructor
    /// @param client The RPC client
    explicit NeoToken(const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    ~NeoToken() = default;

    /// Get unclaimed GAS for an address
    /// @param address The address
    /// @return The unclaimed GAS amount
    [[nodiscard]] int64_t getUnclaimedGas(const std::string& address);

    /// Register as a candidate for consensus node
    /// @param account The account to register
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> registerCandidate(const SharedPtr<Account>& account);

    /// Unregister as a candidate
    /// @param account The account to unregister
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> unregisterCandidate(const SharedPtr<Account>& account);

    /// Vote for candidates
    /// @param account The voting account
    /// @param candidate The candidate public key (or empty to cancel vote)
    /// @return Transaction builder
    SharedPtr<TransactionBuilder> vote(const SharedPtr<Account>& account, const std::string& candidate = "");

    /// Get registered candidates
    /// @return List of candidates with votes
    std::vector<nlohmann::json> getCandidates();

    /// Get committee members
    /// @return List of committee member public keys
    std::vector<std::string> getCommittee();

    /// Get next block validators
    /// @return List of validator public keys
    std::vector<std::string> getNextBlockValidators();

    /// Get GAS per block
    /// @return The GAS reward per block
    [[nodiscard]] int64_t getGasPerBlock();

    /// Get account state (NEO balance and voting info)
    /// @param address The address
    /// @return Account state
    nlohmann::json getAccountState(const std::string& address);

    /// Calculate bonus GAS for holding NEO
    /// @param address The address
    /// @param startBlock Start block for calculation
    /// @param endBlock End block for calculation
    /// @return The bonus GAS amount
    int64_t calculateBonus(const std::string& address, uint32_t startBlock, uint32_t endBlock);
};

} // namespace neocpp
