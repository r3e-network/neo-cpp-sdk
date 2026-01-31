#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace neocpp {

// Basic type aliases
using Byte = uint8_t;
using Bytes = std::vector<uint8_t>;

// Smart pointer aliases for common usage
template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// Common utility functions for byte operations
class ByteUtils {
public:
    /// Convert bytes to hexadecimal string
    static std::string toHex(const Bytes& bytes, bool with_prefix = false);

    /// Convert hexadecimal string to bytes
    static Bytes fromHex(const std::string& hex);

    /// Reverse byte array
    static Bytes reverse(const Bytes& bytes);

    /// Compare two byte arrays
    static bool equal(const Bytes& a, const Bytes& b);

    /// Concatenate multiple byte arrays
    static Bytes concat(const std::vector<Bytes>& arrays);

    /// Get a subset of bytes
    static Bytes subset(const Bytes& bytes, size_t start, size_t length);

    /// Convert integer to bytes (little-endian)
    static Bytes fromInt32LE(int32_t value);
    static Bytes fromInt64LE(int64_t value);

    /// Convert integer to bytes (big-endian)
    static Bytes fromInt32BE(int32_t value);
    static Bytes fromInt64BE(int64_t value);

    /// Convert bytes to integer (little-endian)
    static int32_t toInt32LE(const Bytes& bytes);
    static int64_t toInt64LE(const Bytes& bytes);

    /// Convert bytes to integer (big-endian)
    static int32_t toInt32BE(const Bytes& bytes);
    static int64_t toInt64BE(const Bytes& bytes);
};

// Forward declarations for commonly used types
class Hash160;
class Hash256;
class ECPoint;
class ECKeyPair;
class Transaction;
class Wallet;
class Account;
class SmartContract;

} // namespace neocpp
