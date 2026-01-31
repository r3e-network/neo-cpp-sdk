#include "neocpp/contract/fungible_token.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/transaction/transaction_builder.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <cmath>

namespace neocpp {

FungibleToken::FungibleToken(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client)
    : SmartContract(scriptHash, client), decimals_(0), metadataLoaded_(false) {
}

std::string FungibleToken::getSymbol() {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return symbol_;
}

int FungibleToken::getDecimals() {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return decimals_;
}

int64_t FungibleToken::getTotalSupply() {
    auto result = invokeFunction("totalSupply");
    return result["stack"][0]["value"].get<int64_t>();
}

int64_t FungibleToken::getBalanceOf(const std::string& address) {
    Bytes hashBytes = AddressUtils::addressToScriptHash(address);
    Hash160 scriptHash = Hash160(hashBytes);
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(scriptHash)
    };
    
    auto result = invokeFunction("balanceOf", params);
    return result["stack"][0]["value"].get<int64_t>();
}

SharedPtr<TransactionBuilder> FungibleToken::transfer(const SharedPtr<Account>& from, 
                                                      const std::string& to, 
                                                      int64_t amount,
                                                      const std::string& data) {
    Bytes toHashBytes = AddressUtils::addressToScriptHash(to);
    Hash160 toHash = Hash160(toHashBytes);
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(from->getScriptHash()),
        ContractParameter::hash160(toHash),
        ContractParameter::integer(amount),
        ContractParameter::string(data)
    };
    
    return buildInvokeTx("transfer", params, from);
}

SharedPtr<TransactionBuilder> FungibleToken::transferMultiple(const SharedPtr<Account>& from,
                                                              const std::vector<std::pair<std::string, int64_t>>& recipients,
                                                              const std::string& data) {
    auto builder = std::make_shared<TransactionBuilder>(client_);
    
    for (const auto& [to, amount] : recipients) {
        Bytes toHashBytes = AddressUtils::addressToScriptHash(to);
    Hash160 toHash = Hash160(toHashBytes);
        std::vector<ContractParameter> params = {
            ContractParameter::hash160(from->getScriptHash()),
            ContractParameter::hash160(toHash),
            ContractParameter::integer(amount),
            ContractParameter::string(data)
        };
        
        builder->callContract(scriptHash_, "transfer", params);
    }
    
    builder->addSigner(from);
    return builder;
}

int64_t FungibleToken::toSmallestUnit(double amount) {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return static_cast<int64_t>(amount * std::pow(10, decimals_));
}

double FungibleToken::fromSmallestUnit(int64_t amount) {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return static_cast<double>(amount) / std::pow(10, decimals_);
}

void FungibleToken::loadMetadata() {
    try {
        auto symbolResult = invokeFunction("symbol");
        symbol_ = symbolResult["stack"][0]["value"].get<std::string>();
        
        auto decimalsResult = invokeFunction("decimals");
        decimals_ = decimalsResult["stack"][0]["value"].get<int>();
        
        metadataLoaded_ = true;
    } catch (const std::exception& e) {
        throw IllegalStateException("Failed to load token metadata: " + std::string(e.what()));
    }
}

} // namespace neocpp