#pragma once


#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include <neocpp/types/hash256.hpp>
#include <neocpp/types/neo_vm_state_type.hpp>
#include <neocpp/protocol/core/response/transaction_signer.hpp>
#include <neocpp/protocol/core/response/transaction_attribute.hpp>
#include <neocpp/protocol/core/response/neo_witness.hpp>

namespace neocpp {

/**
 * Transaction response structure returned by various RPC methods
 * Represents a complete Neo blockchain transaction with all metadata
 */
struct Transaction {
    Hash256 hash;
    int size;
    int version;
    int nonce;
    std::string sender;
    std::string sysFee;
    std::string netFee;
    int validUntilBlock;
    std::vector<TransactionSigner> signers;
    std::vector<TransactionAttribute> attributes;
    std::string script;
    std::vector<NeoWitness> witnesses;
    std::optional<Hash256> blockHash;
    std::optional<int> confirmations;
    std::optional<int> blockTime;
    std::optional<NeoVMStateType> vmState;

    Transaction() : size(0), version(0), nonce(0), validUntilBlock(0) {}

    Transaction(const Hash256& hash, int size, int version, int nonce,
                const std::string& sender, const std::string& sysFee,
                const std::string& netFee, int validUntilBlock,
                const std::vector<TransactionSigner>& signers,
                const std::vector<TransactionAttribute>& attributes,
                const std::string& script,
                const std::vector<NeoWitness>& witnesses,
                const std::optional<Hash256>& blockHash = std::nullopt,
                const std::optional<int>& confirmations = std::nullopt,
                const std::optional<int>& blockTime = std::nullopt,
                const std::optional<NeoVMStateType>& vmState = std::nullopt)
        : hash(hash), size(size), version(version), nonce(nonce),
          sender(sender), sysFee(sysFee), netFee(netFee),
          validUntilBlock(validUntilBlock), signers(signers),
          attributes(attributes), script(script), witnesses(witnesses),
          blockHash(blockHash), confirmations(confirmations),
          blockTime(blockTime), vmState(vmState) {}

    bool operator==(const Transaction& other) const {
        return hash == other.hash &&
               size == other.size &&
               version == other.version &&
               nonce == other.nonce &&
               sender == other.sender &&
               sysFee == other.sysFee &&
               netFee == other.netFee &&
               validUntilBlock == other.validUntilBlock &&
               signers == other.signers &&
               attributes == other.attributes &&
               script == other.script &&
               witnesses == other.witnesses &&
               blockHash == other.blockHash &&
               confirmations == other.confirmations &&
               blockTime == other.blockTime &&
               vmState == other.vmState;
    }

    void from_json(const nlohmann::json& j) {
        if (j.contains("hash")) {
            hash = Hash256(j["hash"].get<std::string>());
        }
        if (j.contains("size")) {
            size = j["size"].get<int>();
        }
        if (j.contains("version")) {
            version = j["version"].get<int>();
        }
        if (j.contains("nonce")) {
            nonce = j["nonce"].get<int>();
        }
        if (j.contains("sender")) {
            sender = j["sender"].get<std::string>();
        }
        if (j.contains("sysfee")) {
            sysFee = j["sysfee"].get<std::string>();
        }
        if (j.contains("netfee")) {
            netFee = j["netfee"].get<std::string>();
        }
        if (j.contains("validuntilblock")) {
            validUntilBlock = j["validuntilblock"].get<int>();
        }
        if (j.contains("signers") && j["signers"].is_array()) {
            signers.clear();
            for (const auto& signerJson : j["signers"]) {
                TransactionSigner signer;
                signer.from_json(signerJson);
                signers.push_back(signer);
            }
        }
        if (j.contains("attributes") && j["attributes"].is_array()) {
            attributes.clear();
            for (const auto& attrJson : j["attributes"]) {
                TransactionAttribute attr;
                attr.from_json(attrJson);
                attributes.push_back(attr);
            }
        }
        if (j.contains("script")) {
            script = j["script"].get<std::string>();
        }
        if (j.contains("witnesses") && j["witnesses"].is_array()) {
            witnesses.clear();
            for (const auto& witnessJson : j["witnesses"]) {
                NeoWitness witness;
                witness.from_json(witnessJson);
                witnesses.push_back(witness);
            }
        }
        if (j.contains("blockhash") && !j["blockhash"].is_null()) {
            blockHash = Hash256(j["blockhash"].get<std::string>());
        }
        if (j.contains("confirmations") && !j["confirmations"].is_null()) {
            confirmations = j["confirmations"].get<int>();
        }
        if (j.contains("blocktime") && !j["blocktime"].is_null()) {
            blockTime = j["blocktime"].get<int>();
        }
        if (j.contains("vmstate") && !j["vmstate"].is_null()) {
            vmState = NeoVMStateTypeFromString(j["vmstate"].get<std::string>());
        }
    }

    nlohmann::json to_json() const {
        nlohmann::json j;
        j["hash"] = hash.toString();
        j["size"] = size;
        j["version"] = version;
        j["nonce"] = nonce;
        j["sender"] = sender;
        j["sysfee"] = sysFee;
        j["netfee"] = netFee;
        j["validuntilblock"] = validUntilBlock;

        nlohmann::json signersArray = nlohmann::json::array();
        for (const auto& signer : signers) {
            signersArray.push_back(signer.to_json());
        }
        j["signers"] = signersArray;

        nlohmann::json attributesArray = nlohmann::json::array();
        for (const auto& attr : attributes) {
            attributesArray.push_back(attr.to_json());
        }
        j["attributes"] = attributesArray;

        j["script"] = script;

        nlohmann::json witnessesArray = nlohmann::json::array();
        for (const auto& witness : witnesses) {
            witnessesArray.push_back(witness.to_json());
        }
        j["witnesses"] = witnessesArray;

        if (blockHash.has_value()) {
            j["blockhash"] = blockHash->toString();
        }
        if (confirmations.has_value()) {
            j["confirmations"] = confirmations.value();
        }
        if (blockTime.has_value()) {
            j["blocktime"] = blockTime.value();
        }
        if (vmState.has_value()) {
            j["vmstate"] = NeoVMStateTypeToString(vmState.value());
        }

        return j;
    }
};

} // namespace neocpp

