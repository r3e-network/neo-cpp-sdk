
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/protocol/http_service.hpp"
#include "neocpp/protocol/response_types_impl.hpp"
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/utils/base64.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <sstream>

namespace neocpp {

NeoRpcClient::NeoRpcClient(const std::string& url)
    : url_(url), requestId_(1) {
    httpService_ = std::make_shared<HttpService>(url);
} // namespace neocpp
// Helper method to create JSON-RPC request
static nlohmann::json createRequest(const std::string& method, const nlohmann::json& params, int id) {
    return nlohmann::json{
        {"jsonrpc", "2.0"},
        {"method", method},
        {"params", params},
        {"id", id}
    };
} // namespace neocpp
// Helper method to handle response
static nlohmann::json handleResponse(const nlohmann::json& response) {
    if (response.contains("error")) {
        std::string message = response["error"]["message"].get<std::string>();
        throw RpcException("RPC error: " + message);
    }

    if (!response.contains("result")) {
        throw RpcException("Invalid RPC response: missing result");
    }

    return response["result"];
} // namespace neocpp
SharedPtr<NeoGetVersionResponse> NeoRpcClient::getVersion() {
    auto request = createRequest("getversion", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto versionResponse = std::make_shared<NeoGetVersionResponse>();
    versionResponse->parseJson(result);
    return versionResponse;
} // namespace neocpp
int NeoRpcClient::getConnectionCount() {
    auto request = createRequest("getconnectioncount", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result.get<int>();
} // namespace neocpp
SharedPtr<NeoGetPeersResponse> NeoRpcClient::getPeers() {
    auto request = createRequest("getpeers", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto peersResponse = std::make_shared<NeoGetPeersResponse>();
    peersResponse->parseJson(result);
    return peersResponse;
} // namespace neocpp
nlohmann::json NeoRpcClient::validateAddress(const std::string& address) {
    auto request = createRequest("validateaddress", nlohmann::json::array({address}), requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
Hash256 NeoRpcClient::getBestBlockHash() {
    auto request = createRequest("getbestblockhash", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return Hash256::fromHexString(result.get<std::string>());
} // namespace neocpp
SharedPtr<NeoGetBlockResponse> NeoRpcClient::getBlock(const Hash256& hash, bool verbose) {
    auto params = nlohmann::json::array({hash.toString(), verbose});
    auto request = createRequest("getblock", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto blockResponse = std::make_shared<NeoGetBlockResponse>();
    blockResponse->parseJson(result);
    return blockResponse;
} // namespace neocpp
SharedPtr<NeoGetBlockResponse> NeoRpcClient::getBlock(uint32_t index, bool verbose) {
    auto params = nlohmann::json::array({index, verbose});
    auto request = createRequest("getblock", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto blockResponse = std::make_shared<NeoGetBlockResponse>();
    blockResponse->parseJson(result);
    return blockResponse;
} // namespace neocpp
uint32_t NeoRpcClient::getBlockCount() {
    auto request = createRequest("getblockcount", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result.get<uint32_t>();
} // namespace neocpp
Hash256 NeoRpcClient::getBlockHash(uint32_t index) {
    auto request = createRequest("getblockhash", nlohmann::json::array({index}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return Hash256::fromHexString(result.get<std::string>());
} // namespace neocpp
nlohmann::json NeoRpcClient::getBlockHeader(const Hash256& hash, bool verbose) {
    auto params = nlohmann::json::array({hash.toString(), verbose});
    auto request = createRequest("getblockheader", params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
nlohmann::json NeoRpcClient::getBlockHeader(uint32_t index, bool verbose) {
    auto params = nlohmann::json::array({index, verbose});
    auto request = createRequest("getblockheader", params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
std::vector<std::string> NeoRpcClient::getCommittee() {
    auto request = createRequest("getcommittee", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    std::vector<std::string> committee;
    for (const auto& item : result) {
        committee.push_back(item.get<std::string>());
    }
    return committee;
} // namespace neocpp
SharedPtr<NeoGetContractStateResponse> NeoRpcClient::getContractState(const Hash160& hash) {
    auto request = createRequest("getcontractstate", nlohmann::json::array({hash.toString()}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto contractResponse = std::make_shared<NeoGetContractStateResponse>();
    contractResponse->parseJson(result);
    return contractResponse;
} // namespace neocpp
std::vector<nlohmann::json> NeoRpcClient::getNextBlockValidators() {
    auto request = createRequest("getnextblockvalidators", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    std::vector<nlohmann::json> validators;
    for (const auto& item : result) {
        validators.push_back(item);
    }
    return validators;
} // namespace neocpp
SharedPtr<NeoGetRawTransactionResponse> NeoRpcClient::getRawTransaction(const Hash256& hash, bool verbose) {
    auto params = nlohmann::json::array({hash.toString(), verbose});
    auto request = createRequest("getrawtransaction", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto txResponse = std::make_shared<NeoGetRawTransactionResponse>();
    txResponse->parseJson(result);
    return txResponse;
} // namespace neocpp
SharedPtr<NeoGetApplicationLogResponse> NeoRpcClient::getApplicationLog(const Hash256& hash) {
    auto request = createRequest("getapplicationlog", nlohmann::json::array({hash.toString()}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto logResponse = std::make_shared<NeoGetApplicationLogResponse>();
    logResponse->parseJson(result);
    return logResponse;
} // namespace neocpp
std::string NeoRpcClient::getStorage(const Hash160& scriptHash, const std::string& key) {
    Bytes keyBytes = Hex::decode(key);
    std::string base64Key = Base64::encode(keyBytes);
    auto params = nlohmann::json::array({scriptHash.toString(), base64Key});
    auto request = createRequest("getstorage", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result.get<std::string>();
} // namespace neocpp
uint32_t NeoRpcClient::getTransactionHeight(const Hash256& txId) {
    auto request = createRequest("gettransactionheight", nlohmann::json::array({txId.toString()}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result.get<uint32_t>();
} // namespace neocpp
SharedPtr<NeoGetUnclaimedGasResponse> NeoRpcClient::getUnclaimedGas(const std::string& address) {
    auto request = createRequest("getunclaimedgas", nlohmann::json::array({address}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto gasResponse = std::make_shared<NeoGetUnclaimedGasResponse>();
    gasResponse->parseJson(result);
    return gasResponse;
} // namespace neocpp
SharedPtr<NeoGetNep17BalancesResponse> NeoRpcClient::getNep17Balances(const std::string& address) {
    auto request = createRequest("getnep17balances", nlohmann::json::array({address}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto balancesResponse = std::make_shared<NeoGetNep17BalancesResponse>();
    balancesResponse->parseJson(result);
    return balancesResponse;
} // namespace neocpp
nlohmann::json NeoRpcClient::getNep17Transfers(const std::string& address, uint64_t startTime, uint64_t endTime) {
    auto params = nlohmann::json::array({address, startTime, endTime});
    auto request = createRequest("getnep17transfers", params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
SharedPtr<NeoInvokeResultResponse> NeoRpcClient::invokeFunction(const Hash160& scriptHash,
                                                                const std::string& method,
                                                                const nlohmann::json& params,
                                                                const nlohmann::json& signers) {
    auto requestParams = nlohmann::json::array({
        scriptHash.toString(),
        method,
        params,
        signers
    });

    auto request = createRequest("invokefunction", requestParams, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto invokeResponse = std::make_shared<NeoInvokeResultResponse>();
    invokeResponse->parseJson(result);
    return invokeResponse;
} // namespace neocpp
SharedPtr<NeoInvokeResultResponse> NeoRpcClient::invokeScript(const Bytes& script,
                                                              const nlohmann::json& signers) {
    std::string base64Script = Base64::encode(script);

    auto params = nlohmann::json::array({base64Script, signers});
    auto request = createRequest("invokescript", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto invokeResponse = std::make_shared<NeoInvokeResultResponse>();
    invokeResponse->parseJson(result);
    return invokeResponse;
} // namespace neocpp
SharedPtr<NeoInvokeResultResponse> NeoRpcClient::invokeScript(const std::string& base64Script,
                                                              const nlohmann::json& signers) {
    auto params = nlohmann::json::array({base64Script, signers});
    auto request = createRequest("invokescript", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto invokeResponse = std::make_shared<NeoInvokeResultResponse>();
    invokeResponse->parseJson(result);
    return invokeResponse;
} // namespace neocpp
Hash256 NeoRpcClient::sendRawTransaction(const SharedPtr<Transaction>& transaction) {
    BinaryWriter writer;
    transaction->serialize(writer);
    Bytes rawTx = writer.toArray();
    std::string base64Tx = Base64::encode(rawTx);

    auto request = createRequest("sendrawtransaction", nlohmann::json::array({base64Tx}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    return Hash256::fromHexString(result["hash"].get<std::string>());
} // namespace neocpp
Hash256 NeoRpcClient::sendRawTransaction(const std::string& hex) {
    auto request = createRequest("sendrawtransaction", nlohmann::json::array({hex}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    return Hash256::fromHexString(result["hash"].get<std::string>());
} // namespace neocpp
SharedPtr<NeoGetWalletBalanceResponse> NeoRpcClient::getWalletBalance(const Hash160& assetHash, const std::string& address) {
    auto request = createRequest("getwalletbalance", nlohmann::json::array({assetHash.toString(), address}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);

    auto balanceResponse = std::make_shared<NeoGetWalletBalanceResponse>();
    balanceResponse->parseJson(result);
    return balanceResponse;
} // namespace neocpp
int64_t NeoRpcClient::calculateNetworkFee(const SharedPtr<Transaction>& transaction) {
    BinaryWriter writer;
    transaction->serialize(writer);
    Bytes rawTx = writer.toArray();
    std::string base64Tx = Base64::encode(rawTx);

    auto request = createRequest("calculatenetworkfee", nlohmann::json::array({base64Tx}), requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result["networkfee"].get<int64_t>();
} // namespace neocpp
nlohmann::json NeoRpcClient::getStateHeight() {
    auto request = createRequest("getstateheight", nlohmann::json::array(), requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
nlohmann::json NeoRpcClient::getStateRoot(uint32_t index) {
    auto request = createRequest("getstateroot", nlohmann::json::array({index}), requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
nlohmann::json NeoRpcClient::getProof(const Hash256& rootHash, const Hash160& contractHash, const std::string& key) {
    Bytes keyBytes = Hex::decode(key);
    std::string base64Key = Base64::encode(keyBytes);
    auto params = nlohmann::json::array({rootHash.toString(), contractHash.toString(), base64Key});
    auto request = createRequest("getproof", params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
bool NeoRpcClient::verifyProof(const Hash256& rootHash, const std::string& proof) {
    auto params = nlohmann::json::array({rootHash.toString(), proof});
    auto request = createRequest("verifyproof", params, requestId_++);
    auto response = httpService_->post(request);
    auto result = handleResponse(response);
    return result.get<bool>();
} // namespace neocpp
nlohmann::json NeoRpcClient::findStorage(const Hash160& scriptHash, const std::string& prefix) {
    auto params = nlohmann::json::array({scriptHash.toString(), prefix});
    auto request = createRequest("findstorage", params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
nlohmann::json NeoRpcClient::sendRequest(const std::string& method, const nlohmann::json& params) {
    auto request = createRequest(method, params, requestId_++);
    auto response = httpService_->post(request);
    return handleResponse(response);
} // namespace neocpp
std::vector<nlohmann::json> NeoRpcClient::sendBatch(const std::vector<std::pair<std::string, nlohmann::json>>& requests) {
    nlohmann::json batch = nlohmann::json::array();
    for (const auto& [method, params] : requests) {
        batch.push_back(createRequest(method, params, requestId_++));
    }

    auto response = httpService_->post(batch);

    std::vector<nlohmann::json> results;
    for (const auto& item : response) {
        results.push_back(handleResponse(item));
    }
    return results;
} // namespace neocpp
int NeoRpcClient::getNextRequestId() {
    return requestId_++;
} // namespace neocpp
nlohmann::json NeoRpcClient::buildRequest(const std::string& method, const nlohmann::json& params) {
    return createRequest(method, params, requestId_++);
} // namespace neocpp
nlohmann::json NeoRpcClient::parseResponse(const std::string& response) {
    return nlohmann::json::parse(response);
} // namespace neocpp
void NeoRpcClient::handleError(const nlohmann::json& error) {
    std::string message = "RPC error";
    if (error.contains("message")) {
        message = error["message"].get<std::string>();
    }
    throw RpcException(message);
} // namespace neocpp
nlohmann::json NeoRpcClient::traverseIterator(const std::string& sessionId, const std::string& iteratorId, uint32_t count) {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(sessionId);
    params.push_back(iteratorId);
    params.push_back(count);
    return sendRequest("traverseiterator", params);
} // namespace neocpp
bool NeoRpcClient::terminateSession(const std::string& sessionId) {
    nlohmann::json params = nlohmann::json::array();
    params.push_back(sessionId);
    auto result = sendRequest("terminatesession", params);
    return result.contains("result") && result["result"].get<bool>();
} // namespace neocpp
} // namespace neocpp
