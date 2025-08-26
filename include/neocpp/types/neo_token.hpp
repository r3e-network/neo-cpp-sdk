#pragma once

#include "neocpp/types/hash160.hpp"

namespace neocpp {

/**
 * @brief Constants for the NEO token
 */
class NeoToken {
public:
    /// NEO token script hash on MainNet
    static const Hash160 SCRIPT_HASH;
    
    /// NEO token symbol
    static const std::string SYMBOL;
    
    /// NEO token decimals
    static const int DECIMALS;
    
    /// NEO token total supply
    static const int64_t TOTAL_SUPPLY;
};

} // namespace neocpp