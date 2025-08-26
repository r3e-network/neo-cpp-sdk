#pragma once

#include <vector>
#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash256.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class Signer;
class Witness;
class TransactionAttribute;
class BinaryWriter;
class BinaryReader;
class Account;

/// Represents a Neo transaction
class Transaction : public NeoSerializable {
private:
    uint8_t version_;
    uint32_t nonce_;
    int64_t systemFee_;
    int64_t networkFee_;
    uint32_t validUntilBlock_;
    std::vector<SharedPtr<Signer>> signers_;
    std::vector<SharedPtr<TransactionAttribute>> attributes_;
    Bytes script_;
    std::vector<SharedPtr<Witness>> witnesses_;
    
    mutable Hash256 hash_;
    mutable bool hashCalculated_;
    
public:
    /// Constructor
    Transaction();
    
    /// Destructor
    ~Transaction() = default;
    
    // Getters
    uint8_t getVersion() const { return version_; }
    uint32_t getNonce() const { return nonce_; }
    int64_t getSystemFee() const { return systemFee_; }
    int64_t getNetworkFee() const { return networkFee_; }
    uint32_t getValidUntilBlock() const { return validUntilBlock_; }
    const std::vector<SharedPtr<Signer>>& getSigners() const { return signers_; }
    void clearSigners() { signers_.clear(); }
    const std::vector<SharedPtr<TransactionAttribute>>& getAttributes() const { return attributes_; }
    const Bytes& getScript() const { return script_; }
    const std::vector<SharedPtr<Witness>>& getWitnesses() const { return witnesses_; }
    
    // Setters
    void setVersion(uint8_t version) { version_ = version; hashCalculated_ = false; }
    void setNonce(uint32_t nonce) { nonce_ = nonce; hashCalculated_ = false; }
    void setSystemFee(int64_t fee) { systemFee_ = fee; hashCalculated_ = false; }
    void setNetworkFee(int64_t fee) { networkFee_ = fee; hashCalculated_ = false; }
    void setValidUntilBlock(uint32_t block) { validUntilBlock_ = block; hashCalculated_ = false; }
    void setScript(const Bytes& script) { script_ = script; hashCalculated_ = false; }
    
    /// Add a signer
    void addSigner(const SharedPtr<Signer>& signer);
    
    /// Add an attribute
    void addAttribute(const SharedPtr<TransactionAttribute>& attribute);
    
    /// Add a witness
    void addWitness(const SharedPtr<Witness>& witness);
    
    /// Sign the transaction with an account
    /// @param account The account to sign with
    void sign(const SharedPtr<Account>& account);
    
    /// Get transaction hash
    /// @return The transaction hash
    const Hash256& getHash() const;
    
    /// Get transaction ID (hash in string format)
    /// @return The transaction ID
    std::string getTxId() const;
    
    /// Calculate transaction hash
    /// @return The calculated hash
    Hash256 calculateHash() const;
    
    /// Get the data to be signed for witnesses
    /// @return The signing data
    Bytes getHashData() const;
    
    /// Verify the transaction
    /// @return True if valid
    bool verify() const;
    
    /// Calculate network fee
    /// @return The calculated network fee
    int64_t calculateNetworkFee() const;
    
    // NeoSerializable interface
    size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static SharedPtr<Transaction> deserialize(BinaryReader& reader);
    
    /// Serialize unsigned transaction (without witnesses)
    void serializeUnsigned(BinaryWriter& writer) const;
    
private:
    /// Generate a random nonce for the transaction
    static uint32_t generateNonce();
};

} // namespace neocpp