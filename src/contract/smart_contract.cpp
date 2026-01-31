#include "neocpp/contract/smart_contract.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/protocol/response_types_impl.hpp"
#include "neocpp/transaction/transaction_builder.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"

namespace neocpp {

SmartContract::SmartContract(const Hash160& scriptHash, const SharedPtr<NeoRpcClient>& client)
    : scriptHash_(scriptHash), client_(client) {
    if (!client) {
        throw IllegalArgumentException("RPC client cannot be null");
    }
}

nlohmann::json SmartContract::invokeFunction(const std::string& method, const std::vector<ContractParameter>& params) {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    nlohmann::json jsonParams = paramsToJson(params);
    auto result = client_->invokeFunction(scriptHash_, method, jsonParams);
    return result->getRawJson();
}

SharedPtr<TransactionBuilder> SmartContract::buildInvokeTx(const std::string& method,
                                                           const std::vector<ContractParameter>& params,
                                                           const SharedPtr<Account>& account) {
    auto builder = std::make_shared<TransactionBuilder>(client_);
    builder->callContract(scriptHash_, method, params);
    builder->addSigner(account);
    return builder;
}

nlohmann::json SmartContract::getManifest() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto state = client_->getContractState(scriptHash_);
    auto contractState = state->getContractState();
    return contractState.manifest;
}

std::string SmartContract::getNef() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto state = client_->getContractState(scriptHash_);
    auto contractState = state->getContractState();
    return contractState.nef;
}

bool SmartContract::isDeployed() {
    try {
        auto state = client_->getContractState(scriptHash_);
        return state != nullptr;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("isDeployed check failed: ") + e.what());
        return false;
    } catch (...) {
        LOG_DEBUG("isDeployed check failed with unknown error");
        return false;
    }
}

std::string SmartContract::getName() {
    auto manifest = getManifest();
    return manifest["name"].get<std::string>();
}

std::vector<std::string> SmartContract::getSupportedStandards() {
    auto manifest = getManifest();
    std::vector<std::string> standards;
    
    if (manifest.contains("supportedstandards")) {
        for (const auto& standard : manifest["supportedstandards"]) {
            standards.push_back(standard.get<std::string>());
        }
    }
    
    return standards;
}

nlohmann::json SmartContract::getAbi() {
    auto manifest = getManifest();
    return manifest["abi"];
}

std::vector<nlohmann::json> SmartContract::getMethods() {
    auto abi = getAbi();
    std::vector<nlohmann::json> methods;
    
    if (abi.contains("methods")) {
        for (const auto& method : abi["methods"]) {
            methods.push_back(method);
        }
    }
    
    return methods;
}

std::vector<nlohmann::json> SmartContract::getEvents() {
    auto abi = getAbi();
    std::vector<nlohmann::json> events;
    
    if (abi.contains("events")) {
        for (const auto& event : abi["events"]) {
            events.push_back(event);
        }
    }
    
    return events;
}

nlohmann::json SmartContract::paramsToJson(const std::vector<ContractParameter>& params) {
    nlohmann::json jsonParams = nlohmann::json::array();
    for (const auto& param : params) {
        jsonParams.push_back(param.toRpcJson());
    }
    return jsonParams;
}

} // namespace neocpp