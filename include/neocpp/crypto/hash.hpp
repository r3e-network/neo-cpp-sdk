#pragma once

#include <string>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Hash utilities for cryptographic operations
class HashUtils {
public:
    /// Compute SHA256 hash
    /// @param data The data to hash
    /// @return The SHA256 hash (32 bytes)
    static Bytes sha256(const Bytes& data);

    /// Compute double SHA256 hash (SHA256(SHA256(data)))
    /// @param data The data to hash
    /// @return The double SHA256 hash (32 bytes)
    static Bytes doubleSha256(const Bytes& data);

    /// Compute RIPEMD160 hash
    /// @param data The data to hash
    /// @return The RIPEMD160 hash (20 bytes)
    static Bytes ripemd160(const Bytes& data);

    /// Compute SHA256 then RIPEMD160 hash
    /// @param data The data to hash
    /// @return The hash (20 bytes)
    static Bytes sha256ThenRipemd160(const Bytes& data);

    /// Compute Keccak256 hash (used in Ethereum)
    /// @param data The data to hash
    /// @return The Keccak256 hash (32 bytes)
    static Bytes keccak256(const Bytes& data);

    /// Compute HMAC-SHA256
    /// @param key The HMAC key
    /// @param data The data to authenticate
    /// @return The HMAC-SHA256 (32 bytes)
    static Bytes hmacSha256(const Bytes& key, const Bytes& data);

    /// Compute CRC-32 checksum (IEEE 802.3)
    /// @param data The data to checksum
    /// @return The CRC-32 checksum (4 bytes, little-endian)
    static uint32_t crc32(const Bytes& data);

    /// Constant-time comparison of two byte arrays (prevents timing attacks)
    /// @param a First byte array
    /// @param b Second byte array
    /// @param len Length of both arrays
    /// @return true if arrays are equal, false otherwise
    static bool constantTimeEquals(const uint8_t* a, const uint8_t* b, size_t len);

    /// Constant-time comparison of two byte vectors (prevents timing attacks)
    /// @param a First byte vector
    /// @param b Second byte vector
    /// @return true if vectors are equal, false otherwise
    static bool constantTimeEquals(const Bytes& a, const Bytes& b);
};

} // namespace neocpp
