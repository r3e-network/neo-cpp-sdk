
#include "neocpp/transaction/contract_parameters_context.hpp"
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/script/op_code.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include <algorithm>

namespace neocpp {

ContractParametersContext::ContractParametersContext(const SharedPtr<Transaction>& transaction)
    : transaction_(transaction) {
    if (!transaction) {
        throw IllegalArgumentException("Transaction cannot be null");
    }

    // Initialize verification scripts from signers
    for (const auto& signer : transaction->getSigners()) {
        // Initialize with empty script - will be populated when account signs
        // or can be manually set via setVerificationScript method
        verificationScripts_[signer->getAccount()] = Bytes();
    }
} // namespace neocpp
void ContractParametersContext::addSignature(const SharedPtr<Account>& account, const Bytes& signature) {
    auto scriptHash = account->getScriptHash();
    auto publicKey = account->getKeyPair()->getPublicKey()->getEncoded();

    addSignature(scriptHash, publicKey, signature);

    // Store verification script if not already stored
    if (verificationScripts_[scriptHash].empty()) {
        verificationScripts_[scriptHash] = account->getVerificationScript();
        parseVerificationScript(scriptHash, account->getVerificationScript());
    }
} // namespace neocpp
void ContractParametersContext::addSignature(const Hash160& scriptHash, const Bytes& publicKey, const Bytes& signature) {
    // Add signature to the collection
    signatures_[scriptHash].push_back(signature);
} // namespace neocpp
bool ContractParametersContext::sign(const SharedPtr<Account>& account) {
    auto txHash = transaction_->getHash();
    auto signature = account->sign(txHash.toArray());

    addSignature(account, signature);
    return true;
} // namespace neocpp
bool ContractParametersContext::isComplete() const {
    for (const auto& signer : transaction_->getSigners()) {
        if (!isComplete(signer->getAccount())) {
            return false;
        }
    }
    return true;
} // namespace neocpp
bool ContractParametersContext::isComplete(const Hash160& scriptHash) const {
    int required = getRequiredSignatures(scriptHash);
    int collected = getCollectedSignatures(scriptHash);
    return collected >= required;
} // namespace neocpp
std::vector<SharedPtr<Witness>> ContractParametersContext::getWitnesses() const {
    std::vector<SharedPtr<Witness>> witnesses;

    for (const auto& signer : transaction_->getSigners()) {
        auto witness = getWitness(signer->getAccount());
        if (witness) {
            witnesses.push_back(witness);
        }
    }

    return witnesses;
} // namespace neocpp
SharedPtr<Witness> ContractParametersContext::getWitness(const Hash160& scriptHash) const {
    auto sigIt = signatures_.find(scriptHash);
    auto scriptIt = verificationScripts_.find(scriptHash);

    if (sigIt == signatures_.end() || scriptIt == verificationScripts_.end()) {
        return nullptr;
    }

    // Build invocation script
    ScriptBuilder builder;
    for (const auto& sig : sigIt->second) {
        builder.pushData(sig);
    }

    auto witness = std::make_shared<Witness>();
    witness->setInvocationScript(builder.toArray());
    witness->setVerificationScript(scriptIt->second);

    return witness;
} // namespace neocpp
nlohmann::json ContractParametersContext::toJson() const {
    nlohmann::json json;

    // Serialize transaction to bytes then encode to hex
    BinaryWriter writer;
    transaction_->serialize(writer);
    json["transaction"] = Hex::encode(writer.toArray());

    // Serialize signatures
    nlohmann::json sigs;
    for (const auto& [scriptHash, signatures] : signatures_) {
        nlohmann::json sigArray;
        for (const auto& sig : signatures) {
            sigArray.push_back(Hex::encode(sig));
        }
        sigs[scriptHash.toString()] = sigArray;
    }
    json["signatures"] = sigs;

    // Serialize verification scripts
    nlohmann::json scripts;
    for (const auto& [scriptHash, script] : verificationScripts_) {
        scripts[scriptHash.toString()] = Hex::encode(script);
    }
    json["verificationScripts"] = scripts;

    return json;
} // namespace neocpp
SharedPtr<ContractParametersContext> ContractParametersContext::fromJson(const nlohmann::json& json) {
    // Deserialize transaction
    auto txHex = json["transaction"].get<std::string>();
    auto txBytes = Hex::decode(txHex);
    BinaryReader reader(txBytes);
    auto transaction = Transaction::deserialize(reader);

    auto context = std::make_shared<ContractParametersContext>(transaction);

    // Deserialize signatures
    if (json.contains("signatures")) {
        for (const auto& [hashStr, sigArray] : json["signatures"].items()) {
            Hash160 scriptHash(hashStr);
            for (const auto& sigHex : sigArray) {
                auto sig = Hex::decode(sigHex.get<std::string>());
                context->signatures_[scriptHash].push_back(sig);
            }
        }
    }

    // Deserialize verification scripts
    if (json.contains("verificationScripts")) {
        for (const auto& [hashStr, scriptHex] : json["verificationScripts"].items()) {
            Hash160 scriptHash(hashStr);
            auto script = Hex::decode(scriptHex.get<std::string>());
            context->verificationScripts_[scriptHash] = script;
        }
    }

    return context;
} // namespace neocpp
void ContractParametersContext::parseVerificationScript(const Hash160& scriptHash, const Bytes& script) {
    // Parse verification script to determine if single-sig or multi-sig

    if (script.empty()) {
        return;
    }

    size_t pos = 0;

    // Check for standard single-sig pattern:
    // PUSH21 (0x21) + 33 bytes pubkey + SYSCALL (0x41) + 4 bytes interop hash
    if (script.size() >= 40 && script[0] == 0x21 && script[34] == 0x41) {
        // Single-sig verification script
        scriptInfo_[scriptHash] = {1, 1}; // m=1, n=1
        return;
    }

    // Check for multi-sig pattern:
    // PUSH<m> + PUSH21 <pubkey1> + ... + PUSH21 <pubkeyn> + PUSH<n> + SYSCALL
    if (script.size() > 40) {
        // First byte should be PUSH1-PUSH16 for m value
        if (script[pos] >= 0x11 && script[pos] <= 0x20) {
            int m = script[pos] - 0x10; // m value (required signatures)
            pos++;

            // Count public keys
            int pubkeyCount = 0;
            while (pos < script.size() - 6) { // Need at least 6 bytes for PUSH<n> + SYSCALL
                if (script[pos] == 0x21) { // PUSH21 (33 bytes)
                    pos += 34; // Skip opcode + 33 bytes
                    pubkeyCount++;
                } else {
                    break;
                }
            }

            // Check for PUSH<n> and SYSCALL
            if (pos < script.size() - 5 &&
                script[pos] >= 0x11 && script[pos] <= 0x20 &&
                script[pos + 1] == 0x41) {
                int n = script[pos] - 0x10; // n value (total keys)

                if (pubkeyCount == n && m <= n) {
                    // Valid multi-sig script
                    scriptInfo_[scriptHash] = {m, n};
                    return;
                }
            }
        }
    }

    // Default to single-sig if pattern not recognized
    scriptInfo_[scriptHash] = {1, 1};
} // namespace neocpp
int ContractParametersContext::getRequiredSignatures(const Hash160& scriptHash) const {
    // Parse verification script to determine required signatures
    auto it = scriptInfo_.find(scriptHash);
    if (it != scriptInfo_.end()) {
        return it->second.first; // Return m value
    }

    // If not parsed yet, try to parse from stored script
    auto scriptIt = verificationScripts_.find(scriptHash);
    if (scriptIt != verificationScripts_.end()) {
        const_cast<ContractParametersContext*>(this)->parseVerificationScript(scriptHash, scriptIt->second);
        auto infoIt = scriptInfo_.find(scriptHash);
        if (infoIt != scriptInfo_.end()) {
            return infoIt->second.first;
        }
    }

    return 1; // Default to single-sig
} // namespace neocpp
int ContractParametersContext::getCollectedSignatures(const Hash160& scriptHash) const {
    auto it = signatures_.find(scriptHash);
    if (it == signatures_.end()) {
        return 0;
    }
    return static_cast<int>(it->second.size());
} // namespace neocpp
} // namespace neocpp
