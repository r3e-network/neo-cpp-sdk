#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <map>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/transaction/transaction_attribute.hpp"
#include "neocpp/protocol/response_types_impl.hpp"

namespace neocpp {

// Forward declarations
class Transaction;
class Signer;
class Witness;
class NeoRpcClient;
class Account;
class ContractParameter;

/// Builder class for constructing Neo transactions
class TransactionBuilder {
private:
    SharedPtr<Transaction> transaction_;
    SharedPtr<NeoRpcClient> client_;
    std::vector<SharedPtr<Account>> signingAccounts_;

    // High priority flag
    bool isHighPriority_ = false;

    // Fee handling
    std::function<void(int64_t, int64_t)> feeConsumer_ = nullptr;
    std::shared_ptr<std::exception> feeError_ = nullptr;

    // Additional fees
    int64_t additionalNetworkFee_ = 0;
    int64_t additionalSystemFee_ = 0;

public:
    /// Constructor
    /// @param client The RPC client to use for blockchain queries
    explicit TransactionBuilder(const SharedPtr<NeoRpcClient>& client = nullptr);

    // For testing purposes
    template<typename T>
    explicit TransactionBuilder(const std::shared_ptr<T>& mockClient) {
        transaction_ = std::make_shared<Transaction>();
    }

    /// Destructor
    ~TransactionBuilder() = default;

    /// Set the RPC client
    /// @param client The RPC client
    /// @return Reference to this builder
    TransactionBuilder& setClient(const SharedPtr<NeoRpcClient>& client);

    /// Set the nonce (random value)
    /// @param nonce The nonce value
    /// @return Reference to this builder
    TransactionBuilder& setNonce(uint32_t nonce);

    /// Alias for setNonce
    /// @param nonce The nonce value
    /// @return Reference to this builder
    TransactionBuilder& nonce(uint32_t nonce) { return setNonce(nonce); }

    /// Set a random nonce
    /// @return Reference to this builder
    TransactionBuilder& setRandomNonce();

    /// Set the valid until block
    /// @param block The block number
    /// @return Reference to this builder
    TransactionBuilder& validUntilBlock(uint32_t block);
    TransactionBuilder& setValidUntilBlock(uint32_t block);

    /// Set valid until block relative to current
    /// @param blocksFromNow Number of blocks from current
    /// @return Reference to this builder
    TransactionBuilder& setValidUntilBlockRelative(uint32_t blocksFromNow);

    /// Set the system fee
    /// @param fee The system fee in GAS fractions
    /// @return Reference to this builder
    TransactionBuilder& setSystemFee(int64_t fee);

    /// Set the network fee
    /// @param fee The network fee in GAS fractions
    /// @return Reference to this builder
    TransactionBuilder& setNetworkFee(int64_t fee);

    /// Calculate and set the network fee automatically
    /// @return Reference to this builder
    TransactionBuilder& calculateNetworkFee();

    /// Add a signer to the transaction
    /// @param signer The signer to add
    /// @return Reference to this builder
    TransactionBuilder& addSigner(const SharedPtr<Signer>& signer);

    /// Set signers for the transaction (replaces existing signers)
    /// @param signers The signers to set
    /// @return Reference to this builder
    TransactionBuilder& signers(const std::vector<SharedPtr<Signer>>& signers);

    /// Get the current signers
    /// @return The current signers
    std::vector<SharedPtr<Signer>> getSigners() const;
    /// Add a signer from account
    /// @param account The account to sign with
    /// @param scopes The witness scopes
    /// @return Reference to this builder
    TransactionBuilder& addSigner(const SharedPtr<Account>& account, uint8_t scopes = 0x01);

    /// Get the first signer
    /// @return The first signer if exists, nullptr otherwise
    SharedPtr<Signer> firstSigner() const;

    /// Set high priority flag
    /// @param isHighPriority Whether this is a high priority transaction
    /// @return Reference to this builder
    TransactionBuilder& setHighPriority(bool isHighPriority);

    /// Set additional network fee
    /// @param fee Additional network fee
    /// @return Reference to this builder
    TransactionBuilder& setAdditionalNetworkFee(int64_t fee);

    /// Get additional network fee
    /// @return Additional network fee
    [[nodiscard]] int64_t getAdditionalNetworkFee() const { return additionalNetworkFee_; }

    /// Set additional system fee
    /// @param fee Additional system fee
    /// @return Reference to this builder
    TransactionBuilder& setAdditionalSystemFee(int64_t fee);

    /// Get additional system fee
    /// @return Additional system fee
    [[nodiscard]] int64_t getAdditionalSystemFee() const { return additionalSystemFee_; }

    /// Set the script
    /// @param script The script bytes
    /// @return Reference to this builder
    TransactionBuilder& setScript(const Bytes& script);

    /// Alias for setScript
    /// @param script The script bytes
    /// @return Reference to this builder
    TransactionBuilder& script(const Bytes& script) { return setScript(script); }

    /// Add a transaction attribute
    /// @param attribute The attribute to add
    /// @return Reference to this builder
    TransactionBuilder& addAttribute(const TransactionAttribute& attribute);

    /// Alias for addAttribute
    /// @param attribute The attribute to add
    /// @return Reference to this builder
    TransactionBuilder& attribute(const TransactionAttribute& attribute) { return addAttribute(attribute); }

    /// Call a contract method
    /// @param scriptHash The contract script hash
    /// @param method The method name
    /// @param params The parameters
    /// @return Reference to this builder
    TransactionBuilder& callContract(const Hash160& scriptHash, const std::string& method, const std::vector<ContractParameter>& params = {});

    /// Extend the current script
    /// @param script Additional script bytes
    /// @return Reference to this builder
    TransactionBuilder& extendScript(const Bytes& script);

    /// Build and get unsigned transaction
    /// @return The unsigned transaction
    SharedPtr<Transaction> getUnsignedTransaction();

    /// Call invoke script
    /// @return The invoke script response
    SharedPtr<NeoInvokeResultResponse> callInvokeScript();

    /// Handle case when sender cannot cover fees with a consumer
    /// @param consumer The consumer function
    /// @return Reference to this builder
    TransactionBuilder& doIfSenderCannotCoverFees(const std::function<void(int64_t, int64_t)>& consumer);

    /// Throw error when sender cannot cover fees
    /// @param error The error to throw
    /// @return Reference to this builder
    TransactionBuilder& throwIfSenderCannotCoverFees(const std::shared_ptr<std::exception>& error);

    /// Transfer NEO
    /// @param from The sender account
    /// @param to The recipient address
    /// @param amount The amount of NEO
    /// @return Reference to this builder
    TransactionBuilder& transferNeo(const SharedPtr<Account>& from, const std::string& to, int64_t amount);

    /// Transfer GAS
    /// @param from The sender account
    /// @param to The recipient address
    /// @param amount The amount of GAS (in fractions)
    /// @return Reference to this builder
    TransactionBuilder& transferGas(const SharedPtr<Account>& from, const std::string& to, int64_t amount);

    /// Transfer NEP-17 token
    /// @param tokenHash The token contract hash
    /// @param from The sender account
    /// @param to The recipient address
    /// @param amount The amount to transfer
    /// @param decimals The token decimals
    /// @return Reference to this builder
    TransactionBuilder& transferNep17(const Hash160& tokenHash, const SharedPtr<Account>& from, const std::string& to, int64_t amount, int decimals = 0);

    /// Add a witness
    /// @param witness The witness
    /// @return Reference to this builder
    TransactionBuilder& addWitness(const SharedPtr<Witness>& witness);

    /// Sign the transaction with an account
    /// @param account The account to sign with
    /// @return Reference to this builder
    TransactionBuilder& sign(const SharedPtr<Account>& account);

    /// Sign the transaction with all added signers
    /// @return Reference to this builder
    TransactionBuilder& signWithSigners();

    /// Build the transaction
    /// @return The built transaction
    SharedPtr<Transaction> build();

    /// Build and sign the transaction
    /// @return The signed transaction
    SharedPtr<Transaction> buildAndSign();

    /// Sort signers and witnesses to match Neo ordering rules
    void sortSignersAndWitnesses();

    /// Get the current transaction being built
    /// @return The transaction
    SharedPtr<Transaction> getTransaction() const { return transaction_; }

private:
    /// Initialize transaction with defaults
    void initializeTransaction();

    /// Add witness for account
    void addWitnessForAccount(const SharedPtr<Account>& account);

    /// Sort signers according to Neo rules
    void sortSigners();

    /// Sort witnesses according to signers
    void sortWitnesses();

private:
    /// Check if high priority is allowed
    bool isAllowedForHighPriority();

    /// Check if signers contain multi-sig with committee member
    bool signersContainMultiSigWithCommitteeMember(const std::vector<Hash160>& committee);

    /// Get system fee for script
    [[nodiscard]] int64_t getSystemFeeForScript();

    /// Calculate network fee using RPC
    int64_t calcNetworkFee();

    /// Get sender's GAS balance
    [[nodiscard]] int64_t getSenderGasBalance();

    /// Check if sender can cover fees
    bool canSendCoverFees(int64_t fees);

    /// Create fake verification script for fee calculation
    Bytes createFakeVerificationScript(const SharedPtr<Account>& account);
};

} // namespace neocpp
