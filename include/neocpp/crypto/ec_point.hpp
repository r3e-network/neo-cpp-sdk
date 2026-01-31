#pragma once

#include <string>
#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// Represents a point on an elliptic curve
class ECPoint : public NeoSerializable {
private:
    Bytes encoded_;
    bool isInfinity_;

public:
    /// The point at infinity
    static const ECPoint INFINITY_POINT;

    /// Construct from encoded bytes
    /// @param encoded The encoded point (compressed or uncompressed)
    explicit ECPoint(const Bytes& encoded);

    /// Construct from hex string
    /// @param hex The hex-encoded point
    explicit ECPoint(const std::string& hex);

    /// Construct the point at infinity
    ECPoint();

    /// Copy constructor
    ECPoint(const ECPoint& other) = default;

    /// Move constructor
    ECPoint(ECPoint&&) noexcept = default;

    /// Copy assignment
    ECPoint& operator=(const ECPoint& other) = default;

    /// Move assignment
    ECPoint& operator=(ECPoint&& other) = default;

    /// Destructor
    ~ECPoint() = default;

    /// Get the encoded bytes
    /// @return The encoded point
    const Bytes& getEncoded() const { return encoded_; }

    /// Get the encoded bytes in compressed format
    /// @return The compressed encoded point
    [[nodiscard]] Bytes getEncodedCompressed() const;

    /// Get the encoded bytes in uncompressed format
    /// @return The uncompressed encoded point (0x04 || X || Y)
    [[nodiscard]] Bytes getEncodedUncompressed() const;

    /// Check if this is the point at infinity
    /// @return True if infinity point
    [[nodiscard]] bool isInfinity() const { return isInfinity_; }

    /// Get X coordinate
    /// @return The X coordinate as bytes
    [[nodiscard]] Bytes getX() const;

    /// Get Y coordinate
    /// @return The Y coordinate as bytes
    [[nodiscard]] Bytes getY() const;

    /// Convert to hex string
    /// @return The hex-encoded point
    std::string toHex() const;

    /// Parse from hex string
    /// @param hex The hex string
    /// @return The ECPoint
    static ECPoint fromHex(const std::string& hex);

    /// Check if the point is valid on the curve
    /// @return True if valid
    bool isValid() const;

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static ECPoint deserialize(BinaryReader& reader);

    // Comparison operators
    bool operator==(const ECPoint& other) const;
    bool operator!=(const ECPoint& other) const;
    bool operator<(const ECPoint& other) const;
};

} // namespace neocpp
