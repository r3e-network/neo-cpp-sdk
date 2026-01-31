#pragma once

#include <string>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Address utilities for Neo addresses
class AddressUtils {
public:
    /// Convert a script hash to an address
    /// @param scriptHash The script hash in big-endian order
    /// @return The Neo address
    static std::string scriptHashToAddress(const Bytes& scriptHash);

    /// Convert an address to a script hash
    /// @param address The Neo address
    /// @return The script hash in big-endian order
    static Bytes addressToScriptHash(const std::string& address);

    /// Validate a Neo address
    /// @param address The address to validate
    /// @return True if valid, false otherwise
    static bool isValidAddress(const std::string& address);

    /// Get the address version byte
    /// @return The version byte for Neo N3 addresses
    static uint8_t getAddressVersion();
};

} // namespace neocpp
