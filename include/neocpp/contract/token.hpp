#pragma once

#include "neocpp/contract/smart_contract.hpp"
#include <string>

namespace neocpp {

/// Base class for all token contracts
class Token : public SmartContract {
public:
    /// Constructor
    /// @param scriptHash The contract script hash
    /// @param client The RPC client
    Token(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client);

    /// Destructor
    virtual ~Token() = default;

    /// Get token symbol
    /// @return The token symbol
    virtual std::string getSymbol() = 0;

    /// Get token decimals
    /// @return The number of decimals
    virtual int getDecimals() = 0;

    /// Get total supply
    /// @return The total supply
    virtual int64_t getTotalSupply() = 0;

    /// Get balance of an address
    /// @param address The address
    /// @return The balance
    virtual int64_t getBalanceOf(const std::string& address) = 0;
};

} // namespace neocpp
