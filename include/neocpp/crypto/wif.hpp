#pragma once

#include <string>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// WIF (Wallet Import Format) utilities
class WIF {
public:
    /// Encode a private key to WIF format
    /// @param privateKey The private key bytes (32 bytes)
    /// @return The WIF-encoded string
    static std::string encode(const Bytes& privateKey);

    /// Decode a WIF string to private key
    /// @param wif The WIF-encoded string
    /// @return The private key bytes (32 bytes)
    static Bytes decode(const std::string& wif);

    /// Validate a WIF string
    /// @param wif The WIF string to validate
    /// @return True if valid, false otherwise
    static bool isValid(const std::string& wif);

private:
    static const uint8_t WIF_VERSION = 0x80;
    static const uint8_t COMPRESSED_FLAG = 0x01;
};

} // namespace neocpp
