#pragma once

#include <vector>
#include <memory>
#include "neocpp/types/types.hpp"

namespace neocpp {

// Forward declarations
class ECPublicKey;

/// Verification script for transaction witnesses
class VerificationScript {
private:
    Bytes script_;

public:
    /// Constructor from script bytes
    /// @param script The verification script
    explicit VerificationScript(const Bytes& script);

    /// Create single signature verification script
    /// @param publicKey The public key
    /// @return The verification script
    static VerificationScript fromPublicKey(const SharedPtr<ECPublicKey>& publicKey);

    /// Create multi-signature verification script
    /// @param publicKeys The public keys
    /// @param signingThreshold The minimum signatures required
    /// @return The verification script
    static VerificationScript fromPublicKeys(const std::vector<SharedPtr<ECPublicKey>>& publicKeys,
                                            int signingThreshold);

    /// Get the script bytes
    /// @return The script
    const Bytes& getScript() const { return script_; }

    /// Get the script hash
    /// @return The script hash
    [[nodiscard]] Bytes getScriptHash() const;

    /// Check if this is a single signature script
    /// @return True if single signature
    bool isSingleSig() const;

    /// Check if this is a multi-signature script
    /// @return True if multi-signature
    bool isMultiSig() const;

    /// Get the signing threshold for multi-sig
    /// @return The threshold (0 if not multi-sig)
    [[nodiscard]] int getSigningThreshold() const;

    /// Get the public keys from the script
    /// @return The public keys
    std::vector<SharedPtr<ECPublicKey>> getPublicKeys() const;
};

} // namespace neocpp
