#pragma once

#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// Represents a transaction witness
class Witness : public NeoSerializable {
private:
    Bytes invocationScript_;
    Bytes verificationScript_;

public:
    /// Constructor
    Witness() = default;

    /// Constructor with scripts
    /// @param invocationScript The invocation script
    /// @param verificationScript The verification script
    Witness(const Bytes& invocationScript, const Bytes& verificationScript);

    /// Destructor
    ~Witness() = default;

    // Getters
    const Bytes& getInvocationScript() const { return invocationScript_; }
    const Bytes& getVerificationScript() const { return verificationScript_; }

    // Setters
    void setInvocationScript(const Bytes& script) { invocationScript_ = script; }
    void setVerificationScript(const Bytes& script) { verificationScript_ = script; }

    /// Get the script hash of this witness
    /// @return The script hash
    [[nodiscard]] Hash160 getScriptHash() const;

    /// Create a witness from a signature
    /// @param signature The signature
    /// @param publicKey The public key
    /// @return The witness
    static SharedPtr<Witness> fromSignature(const Bytes& signature, const Bytes& publicKey);

    /// Create a multi-signature witness
    /// @param signatures The signatures
    /// @param publicKeys The public keys
    /// @param signingThreshold The minimum number of signatures required
    /// @return The witness
    static SharedPtr<Witness> fromMultiSignature(const std::vector<Bytes>& signatures,
                                                  const std::vector<Bytes>& publicKeys,
                                                  int signingThreshold);

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static SharedPtr<Witness> deserialize(BinaryReader& reader);

    // Comparison operators
    bool operator==(const Witness& other) const;
    bool operator!=(const Witness& other) const;
};

} // namespace neocpp
