#include "neocpp/types/gas_token.hpp"

namespace neocpp {

const Hash160 GasToken::SCRIPT_HASH = Hash160("0xd2a4cff31913016155e38e474a2c06d08be276cf");
const std::string GasToken::SYMBOL = "GAS";
const int GasToken::DECIMALS = 8;
const int64_t GasToken::TOTAL_SUPPLY = 5200000000000000LL; // 52,000,000 GAS with 8 decimals

} // namespace neocpp