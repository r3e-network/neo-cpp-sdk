#pragma once

#include <string>
#include <array>
#include "neocpp/types/types.hpp"
#include "neocpp/neo_constants.hpp"

namespace neocpp {

/// Represents an ECDSA signature
class ECDSASignature {
private:
    std::array<uint8_t, NeoConstants::SIGNATURE_SIZE> signature_;
    bool isEmpty_;  ///< True if signature was created from empty bytes

public:
    /// Construct from signature bytes
    /// @param signature The signature bytes (64 bytes: 32 bytes R + 32 bytes S)
    explicit ECDSASignature(const Bytes& signature);

    /// Construct from array
    /// @param signature The signature array
    explicit ECDSASignature(const std::array<uint8_t, NeoConstants::SIGNATURE_SIZE>& signature);

    /// Construct from R and S components
    /// @param r The R component (32 bytes)
    /// @param s The S component (32 bytes)
    ECDSASignature(const Bytes& r, const Bytes& s);

    /// Construct from hex string
    /// @param hex The hex-encoded signature
    explicit ECDSASignature(const std::string& hex);

    /// Get the signature bytes
    /// @return The signature as bytes
    [[nodiscard]] Bytes getBytes() const;

    /// Get the R component
    /// @return The R component (first 32 bytes)
    [[nodiscard]] Bytes getR() const;

    /// Get the S component
    /// @return The S component (last 32 bytes)
    [[nodiscard]] Bytes getS() const;

    /// Convert to hex string
    /// @return The hex-encoded signature
    std::string toHex() const;

    /// Convert to DER format
    /// @return The DER-encoded signature
    Bytes toDER() const;

    /// Parse from DER format
    /// @param der The DER-encoded signature
    /// @return The ECDSASignature
    static ECDSASignature fromDER(const Bytes& der);

    /// Check if the signature is canonical
    /// @return True if canonical
    bool isCanonical() const;

    /// Make the signature canonical if it isn't already
    /// @return A canonical version of this signature
    ECDSASignature makeCanonical() const;

    // Comparison operators
    bool operator==(const ECDSASignature& other) const;
    bool operator!=(const ECDSASignature& other) const;
};

} // namespace neocpp
