#pragma once

#include <string>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Hexadecimal encoding and decoding utilities
class Hex {
public:
    /// Encode bytes to hexadecimal string
    /// @param data The data to encode
    /// @param uppercase Whether to use uppercase letters
    /// @return The hex encoded string (without 0x prefix)
    static std::string encode(const Bytes& data, bool uppercase = false);

    /// Decode hexadecimal string to bytes
    /// @param hex The hex string (with or without 0x prefix)
    /// @return The decoded bytes
    static Bytes decode(const std::string& hex);

    /// Check if a string is valid hexadecimal
    /// @param str The string to check
    /// @return True if valid hex, false otherwise
    static bool isValid(const std::string& str);

    /// Add 0x prefix if not present
    /// @param hex The hex string
    /// @return The hex string with 0x prefix
    static std::string withPrefix(const std::string& hex);

    /// Remove 0x prefix if present
    /// @param hex The hex string
    /// @return The hex string without 0x prefix
    static std::string withoutPrefix(const std::string& hex);
};

} // namespace neocpp
