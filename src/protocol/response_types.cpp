#include "neocpp/protocol/response_types_impl.hpp"
#include "neocpp/protocol/stack_item.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

// NeoGetVersionResponse
void NeoGetVersionResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("tcpport")) {
        tcpPort_ = json["tcpport"].get<int>();
    }
    if (json.contains("wsport")) {
        wsPort_ = json["wsport"].get<int>();
    }
    if (json.contains("nonce")) {
        nonce_ = json["nonce"].get<uint32_t>();
    }
    if (json.contains("useragent")) {
        userAgent_ = json["useragent"].get<std::string>();
    }
    if (json.contains("protocol")) {
        protocol_ = json["protocol"];
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetPeersResponse
void NeoGetPeersResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("connected")) {
        connected_ = json["connected"];
    }
    if (json.contains("unconnected")) {
        unconnected_ = json["unconnected"];
    }
    if (json.contains("bad")) {
        bad_ = json["bad"];
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetBlockResponse
void NeoGetBlockResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("hash")) {
        hash_ = Hash256::fromHexString(json["hash"].get<std::string>());
    }
    if (json.contains("size")) {
        size_ = json["size"].get<int>();
    }
    if (json.contains("version")) {
        version_ = json["version"].get<int>();
    }
    if (json.contains("previousblockhash")) {
        previousBlockHash_ = Hash256::fromHexString(json["previousblockhash"].get<std::string>());
    }
    if (json.contains("merkleroot")) {
        merkleRoot_ = Hash256::fromHexString(json["merkleroot"].get<std::string>());
    }
    if (json.contains("time")) {
        time_ = json["time"].get<uint64_t>();
    }
    if (json.contains("index")) {
        index_ = json["index"].get<uint32_t>();
    }
    if (json.contains("nextconsensus")) {
        nextConsensus_ = json["nextconsensus"].get<std::string>();
    }
    if (json.contains("witnesses")) {
        witnesses_ = json["witnesses"];
    }
    if (json.contains("tx")) {
        transactions_ = json["tx"];
    }
    if (json.contains("confirmations")) {
        confirmations_ = json["confirmations"].get<int>();
    }
    if (json.contains("nextblockhash")) {
        nextBlockHash_ = Hash256::fromHexString(json["nextblockhash"].get<std::string>());
        hasNextBlockHash_ = true;
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetRawTransactionResponse
void NeoGetRawTransactionResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("hash")) {
        hash_ = Hash256::fromHexString(json["hash"].get<std::string>());
    }
    if (json.contains("size")) {
        size_ = json["size"].get<int>();
    }
    if (json.contains("version")) {
        version_ = json["version"].get<int>();
    }
    if (json.contains("nonce")) {
        nonce_ = json["nonce"].get<uint32_t>();
    }
    if (json.contains("sender")) {
        sender_ = json["sender"].get<std::string>();
    }
    if (json.contains("sysfee")) {
        sysfee_ = json["sysfee"].get<std::string>();
    }
    if (json.contains("netfee")) {
        netfee_ = json["netfee"].get<std::string>();
    }
    if (json.contains("validuntilblock")) {
        validUntilBlock_ = json["validuntilblock"].get<uint32_t>();
    }
    if (json.contains("signers")) {
        signers_ = json["signers"];
    }
    if (json.contains("attributes")) {
        attributes_ = json["attributes"];
    }
    if (json.contains("witnesses")) {
        witnesses_ = json["witnesses"];
    }
    if (json.contains("script")) {
        script_ = json["script"].get<std::string>();
    }
    if (json.contains("blockhash")) {
        blockHash_ = Hash256::fromHexString(json["blockhash"].get<std::string>());
    }
    if (json.contains("confirmations")) {
        confirmations_ = json["confirmations"].get<int>();
    }
    if (json.contains("blocktime")) {
        blockTime_ = json["blocktime"].get<uint64_t>();
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetApplicationLogResponse
void NeoGetApplicationLogResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("txid")) {
        txid_ = json["txid"].get<std::string>();
    }
    if (json.contains("executions")) {
        executions_ = json["executions"];
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetContractStateResponse
void NeoGetContractStateResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("id")) {
        id_ = json["id"].get<int>();
    }
    if (json.contains("updatecounter")) {
        updateCounter_ = json["updatecounter"].get<int>();
    }
    if (json.contains("hash")) {
        hash_ = Hash160(json["hash"].get<std::string>());
    }
    if (json.contains("nef")) {
        nef_ = json["nef"];
    }
    if (json.contains("manifest")) {
        manifest_ = json["manifest"];
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetNep17BalancesResponse
void NeoGetNep17BalancesResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("address")) {
        address_ = json["address"].get<std::string>();
    }
    if (json.contains("balance")) {
        for (const auto& balance : json["balance"]) {
            NeoNep17Balance bal;
            bal.assetHash = Hash160(balance["assethash"].get<std::string>());
            bal.amount = balance["amount"].get<std::string>();
            bal.lastUpdatedBlock = balance["lastupdatedblock"].get<uint32_t>();
            balances_.push_back(bal);
        }
    }
    rawJson_ = json;
} // namespace neocpp
// NeoInvokeResultResponse
void NeoInvokeResultResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("script")) {
        script_ = json["script"].get<std::string>();
    }
    if (json.contains("state")) {
        state_ = json["state"].get<std::string>();
    }
    if (json.contains("gasconsumed")) {
        gasConsumed_ = json["gasconsumed"].get<std::string>();
    }
    if (json.contains("exception")) {
        exception_ = json["exception"].get<std::string>();
        hasException_ = true;
    }
    if (json.contains("stack")) {
        for (const auto& item : json["stack"]) {
            stack_.push_back(StackItem::fromJson(item));
        }
    }
    if (json.contains("tx")) {
        tx_ = json["tx"].get<std::string>();
    }
    if (json.contains("notifications")) {
        notifications_ = json["notifications"];
    }
    if (json.contains("diagnostics")) {
        diagnostics_ = json["diagnostics"];
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetUnclaimedGasResponse
void NeoGetUnclaimedGasResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("unclaimed")) {
        unclaimed_ = json["unclaimed"].get<std::string>();
    }
    if (json.contains("address")) {
        address_ = json["address"].get<std::string>();
    }
    rawJson_ = json;
} // namespace neocpp
// NeoGetWalletBalanceResponse
void NeoGetWalletBalanceResponse::parseJson(const nlohmann::json& json) {
    if (json.contains("balance")) {
        balance_ = json["balance"].get<std::string>();
    }
    rawJson_ = json;
} // namespace neocpp
} // namespace neocpp
