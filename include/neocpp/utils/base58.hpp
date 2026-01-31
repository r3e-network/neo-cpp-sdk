#pragma once

#include <string>
#include <vector>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Base58 encoding and decoding utilities
class Base58 {
public:
    /// Encode bytes to Base58 string
    /// @param data The data to encode
    /// @return The Base58 encoded string
    static std::string encode(const Bytes& data);

    /// Decode Base58 string to bytes
    /// @param encoded The Base58 encoded string
    /// @return The decoded bytes
    static Bytes decode(const std::string& encoded);

    /// Encode bytes to Base58Check string (includes checksum)
    /// @param data The data to encode
    /// @return The Base58Check encoded string
    static std::string encodeCheck(const Bytes& data);

    /// Decode Base58Check string to bytes (verifies checksum)
    /// @param encoded The Base58Check encoded string
    /// @return The decoded bytes
    static Bytes decodeCheck(const std::string& encoded);

private:
    static const char* ALPHABET;
    static const int BASE;

    /// Calculate checksum for Base58Check
    static Bytes calculateChecksum(const Bytes& data);

    /// Verify checksum for Base58Check
    static bool verifyChecksum(const Bytes& dataWithChecksum);
};

} // namespace neocpp
