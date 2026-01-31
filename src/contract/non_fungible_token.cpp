#include "neocpp/contract/non_fungible_token.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/transaction/transaction_builder.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"

namespace neocpp {

NonFungibleToken::NonFungibleToken(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client)
    : SmartContract(scriptHash, client), decimals_(0), metadataLoaded_(false) {
}

std::string NonFungibleToken::getSymbol() {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return symbol_;
}

int NonFungibleToken::getDecimals() {
    if (!metadataLoaded_) {
        loadMetadata();
    }
    return decimals_;
}

int64_t NonFungibleToken::getTotalSupply() {
    auto result = invokeFunction("totalSupply");
    return result["stack"][0]["value"].get<int64_t>();
}

int64_t NonFungibleToken::getBalanceOf(const std::string& address) {
    Bytes hashBytes = AddressUtils::addressToScriptHash(address);
    Hash160 scriptHash = Hash160(hashBytes);
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(scriptHash)
    };
    
    auto result = invokeFunction("balanceOf", params);
    return result["stack"][0]["value"].get<int64_t>();
}

std::vector<std::string> NonFungibleToken::getTokensOf(const std::string& address) {
    Bytes hashBytes = AddressUtils::addressToScriptHash(address);
    Hash160 scriptHash = Hash160(hashBytes);
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(scriptHash)
    };
    
    auto result = invokeFunction("tokensOf", params);
    std::vector<std::string> tokens;
    
    if (result["stack"][0]["type"] == "Array") {
        for (const auto& item : result["stack"][0]["value"]) {
            tokens.push_back(item["value"].get<std::string>());
        }
    }
    
    return tokens;
}

std::string NonFungibleToken::getOwnerOf(const std::string& tokenId) {
    std::vector<ContractParameter> params = {
        ContractParameter::string(tokenId)
    };
    
    auto result = invokeFunction("ownerOf", params);
    return result["stack"][0]["value"].get<std::string>();
}

nlohmann::json NonFungibleToken::getProperties(const std::string& tokenId) {
    std::vector<ContractParameter> params = {
        ContractParameter::string(tokenId)
    };
    
    return invokeFunction("properties", params);
}

std::vector<std::string> NonFungibleToken::getAllTokens() {
    auto result = invokeFunction("tokens");
    std::vector<std::string> tokens;
    
    if (result["stack"][0]["type"] == "InteropInterface") {
        // Iterator returned - this requires session-based RPC calls to traverse
        // Since iterators are session-specific and require stateful connection,
        // return empty vector. Callers should use paginated methods if available.
        return tokens;
    }
    
    // If result is an array, parse it
    if (result["stack"][0]["type"] == "Array") {
        for (const auto& item : result["stack"][0]["value"]) {
            if (item["type"] == "ByteString") {
                auto base64 = item["value"].get<std::string>();
                // Decode base64 to get token ID
                tokens.push_back(base64);
            }
        }
    }
    
    return tokens;
}

SharedPtr<TransactionBuilder> NonFungibleToken::transfer(const SharedPtr<Account>& from, 
                                                         const std::string& to, 
                                                         const std::string& tokenId,
                                                         const std::string& data) {
    auto builder = std::make_shared<TransactionBuilder>(client_);
    
    auto fromHash = from->getScriptHash();
    auto toBytes = AddressUtils::addressToScriptHash(to);
    Hash160 toHash(toBytes);
    
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(toHash),
        ContractParameter::byteArray(Bytes(tokenId.begin(), tokenId.end())),
        ContractParameter::any()  // data parameter
    };
    
    builder->callContract(scriptHash_, "transfer", params);
    builder->addSigner(from);
    
    return builder;
}

bool NonFungibleToken::isDivisible() {
    try {
        // Try to call decimals - divisible NFTs have decimals > 0
        auto result = invokeFunction("decimals");
        return result["stack"][0]["value"].get<int>() > 0;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("isDivisible check failed: ") + e.what());
        return false;
    } catch (...) {
        LOG_DEBUG("isDivisible check failed with unknown error");
        return false;
    }
}

int64_t NonFungibleToken::getBalanceOf(const std::string& address, const std::string& tokenId) {
    Bytes hashBytes = AddressUtils::addressToScriptHash(address);
    Hash160 scriptHash = Hash160(hashBytes);
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(scriptHash),
        ContractParameter::string(tokenId)
    };
    
    auto result = invokeFunction("balanceOf", params);
    return result["stack"][0]["value"].get<int64_t>();
}

SharedPtr<TransactionBuilder> NonFungibleToken::transfer(const SharedPtr<Account>& from,
                                                         const std::string& to,
                                                         int64_t amount,
                                                         const std::string& tokenId,
                                                         const std::string& data) {
    auto builder = std::make_shared<TransactionBuilder>(client_);
    
    auto fromHash = from->getScriptHash();
    auto toBytes = AddressUtils::addressToScriptHash(to);
    Hash160 toHash(toBytes);
    
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(fromHash),
        ContractParameter::hash160(toHash),
        ContractParameter::integer(amount),
        ContractParameter::byteArray(Bytes(tokenId.begin(), tokenId.end())),
        ContractParameter::any()  // data parameter
    };
    
    builder->callContract(scriptHash_, "transfer", params);
    builder->addSigner(from);
    
    return builder;
}

void NonFungibleToken::loadMetadata() {
    try {
        auto symbolResult = invokeFunction("symbol");
        symbol_ = symbolResult["stack"][0]["value"].get<std::string>();
        
        try {
            auto decimalsResult = invokeFunction("decimals");
            decimals_ = decimalsResult["stack"][0]["value"].get<int>();
        } catch (const std::exception& e) {
            LOG_DEBUG(std::string("Failed to get decimals, defaulting to 0: ") + e.what());
            decimals_ = 0; // Default for NFTs
        } catch (...) {
            LOG_DEBUG("Failed to get decimals with unknown error, defaulting to 0");
            decimals_ = 0; // Default for NFTs
        }
        
        metadataLoaded_ = true;
    } catch (const std::exception& e) {
        throw IllegalStateException("Failed to load NFT metadata: " + std::string(e.what()));
    }
}

} // namespace neocpp