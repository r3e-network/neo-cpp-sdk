#pragma once

#include <string>
#include <array>
#include <vector>
#include "neocpp/types/types.hpp"
#include "neocpp/neo_constants.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// A Hash256 is a 32 bytes long hash created from some data by applying SHA-256.
/// These hashes are mostly used for obtaining transaction or block hashes.
class Hash256 : public NeoSerializable {
private:
    /// The hash is stored as an unsigned integer in big-endian order.
    std::array<uint8_t, NeoConstants::HASH256_SIZE> hash_;

public:
    /// A zero-value hash.
    static const Hash256 ZERO;

    /// Constructs a new hash with 32 zero bytes.
    Hash256();

    /// Constructs a new hash from the given byte array. The byte array must be in big-endian order and 256 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash256(const Bytes& hash);

    /// Constructs a new hash from the given array. The array must be in big-endian order and 256 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash256(const std::array<uint8_t, NeoConstants::HASH256_SIZE>& hash);

    /// Constructs a new hash from the given hexadecimal string. The string must be in big-endian order and 256 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash256(const std::string& hash);

    /// Creates a Hash256 from a hex string
    /// @param hex The hex string (with or without 0x prefix)
    /// @return The Hash256
    static Hash256 fromHexString(const std::string& hex);

    /// Copy constructor
    Hash256(const Hash256& other) = default;

    /// Move constructor
    Hash256(Hash256&&) noexcept = default;

    /// Copy assignment operator
    Hash256& operator=(const Hash256& other) = default;

    /// Move assignment operator
    Hash256& operator=(Hash256&& other) = default;

    /// Destructor
    ~Hash256() = default;

    /// @return The hash as hexadecimal string in big-endian order without the '0x' prefix.
    [[nodiscard]] std::string toString() const;

    /// @return The hash as a byte array in big-endian order
    [[nodiscard]] Bytes toArray() const;

    /// @return The hash as a byte array in little-endian order
    [[nodiscard]] Bytes toLittleEndianArray() const;

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    [[nodiscard]] static Hash256 deserialize(BinaryReader& reader);

    // Comparison operators
    bool operator==(const Hash256& other) const;
    bool operator!=(const Hash256& other) const;
    bool operator<(const Hash256& other) const;
    bool operator<=(const Hash256& other) const;
    bool operator>(const Hash256& other) const;
    bool operator>=(const Hash256& other) const;

    // Hash function for use in std::unordered_map/set
    struct Hasher {
        size_t operator()(const Hash256& hash) const;
    };
};

} // namespace neocpp
