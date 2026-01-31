#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/neo_constants.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class ECPublicKey;
class BinaryWriter;
class BinaryReader;

/// A Hash160 is a 20 bytes long hash created from some data by first applying SHA-256 and then RIPEMD-160.
/// These hashes are mostly used for obtaining the script hash of a smart contract or an account.
class Hash160 : public NeoSerializable {
private:
    /// The hash is stored as an unsigned integer in big-endian order.
    std::array<uint8_t, NeoConstants::HASH160_SIZE> hash_;

public:
    /// A zero-value hash.
    static const Hash160 ZERO;

    /// Constructs a new hash with 20 zero bytes.
    Hash160();

    /// Constructs a new hash from the given byte array. The byte array must be in big-endian order and 160 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash160(const Bytes& hash);

    /// Constructs a new hash from the given array. The array must be in big-endian order and 160 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash160(const std::array<uint8_t, NeoConstants::HASH160_SIZE>& hash);

    /// Constructs a new hash from the given hexadecimal string. The string must be in big-endian order and 160 bits long.
    /// @param hash The hash in big-endian order
    explicit Hash160(const std::string& hash);

    /// Copy constructor
    Hash160(const Hash160& other) = default;

    /// Move constructor
    Hash160(Hash160&&) noexcept = default;

    /// Copy assignment operator
    Hash160& operator=(const Hash160& other) = default;

    /// Move assignment operator
    Hash160& operator=(Hash160&& other) = default;

    /// Destructor
    ~Hash160() = default;

    /// @return The script hash as a hexadecimal string in big-endian order without the '0x' prefix.
    [[nodiscard]] std::string toString() const;

    /// @return The script hash as a byte array in big-endian order
    [[nodiscard]] Bytes toArray() const;

    /// @return The script hash as a byte array in little-endian order
    [[nodiscard]] Bytes toLittleEndianArray() const;

    /// @return The address corresponding to this script hash
    [[nodiscard]] std::string toAddress() const;

    /// Creates a script hash from the given address.
    /// @param address The address from which to derive the script hash
    /// @return The script hash
    [[nodiscard]] static Hash160 fromAddress(const std::string& address);

    /// Creates a script hash from the given script in byte array form.
    /// @param script The script to calculate the script hash for
    /// @return The script hash
    [[nodiscard]] static Hash160 fromScript(const Bytes& script);

    /// Creates a script hash from the given script in hexadecimal string form.
    /// @param script The script to calculate the script hash for
    /// @return The script hash
    [[nodiscard]] static Hash160 fromScript(const std::string& script);

    /// Creates a script hash from the given public key.
    /// @param encodedPublicKey The encoded public key
    /// @return The script hash
    static Hash160 fromPublicKey(const Bytes& encodedPublicKey);

    /// Creates a script hash from multiple public keys for multi-signature.
    /// @param pubKeys The public keys
    /// @param signingThreshold The minimum number of signatures required
    /// @return The script hash
    static Hash160 fromPublicKeys(const std::vector<SharedPtr<ECPublicKey>>& pubKeys, int signingThreshold);

    // NeoSerializable interface
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static Hash160 deserialize(BinaryReader& reader);

    // Comparison operators
    bool operator==(const Hash160& other) const;
    bool operator!=(const Hash160& other) const;
    bool operator<(const Hash160& other) const;
    bool operator<=(const Hash160& other) const;
    bool operator>(const Hash160& other) const;
    bool operator>=(const Hash160& other) const;

    // Hash function for use in std::unordered_map/set
    struct Hasher {
        size_t operator()(const Hash160& hash) const;
    };
};

} // namespace neocpp
