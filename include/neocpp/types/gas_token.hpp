#pragma once

#include "neocpp/types/hash160.hpp"

namespace neocpp {

/**
 * @brief Constants for the GAS token
 */
class GasToken {
public:
    /// GAS token script hash on MainNet
    static const Hash160 SCRIPT_HASH;

    /// GAS token symbol
    static const std::string SYMBOL;

    /// GAS token decimals
    static const int DECIMALS;

    /// GAS token total supply
    static const int64_t TOTAL_SUPPLY;
};

} // namespace neocpp
