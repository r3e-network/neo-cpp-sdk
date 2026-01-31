#pragma once

#include <string>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Base64 encoding and decoding utilities
class Base64 {
public:
    /// Encode bytes to Base64 string
    /// @param data The data to encode
    /// @return The Base64 encoded string
    static std::string encode(const Bytes& data);

    /// Decode Base64 string to bytes
    /// @param encoded The Base64 encoded string
    /// @return The decoded bytes
    static Bytes decode(const std::string& encoded);

    /// Check if a string is valid Base64
    /// @param str The string to check
    /// @return True if valid Base64, false otherwise
    static bool isValid(const std::string& str);
};

} // namespace neocpp
