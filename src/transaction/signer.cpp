
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness_rule.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

Signer::Signer(const Hash160& account, WitnessScope scopes)
    : account_(account), scopes_(scopes) {
} // namespace neocpp
void Signer::addAllowedContract(const Hash160& contractHash) {
    if (allowedContracts_.size() >= NeoConstants::MAX_SIGNER_SUBITEMS) {
        throw TransactionException("Maximum number of allowed contracts exceeded");
    }
    allowedContracts_.push_back(contractHash);
} // namespace neocpp
void Signer::addAllowedGroup(const Bytes& groupPubKey) {
    if (allowedGroups_.size() >= NeoConstants::MAX_SIGNER_SUBITEMS) {
        throw TransactionException("Maximum number of allowed groups exceeded");
    }
    allowedGroups_.push_back(groupPubKey);
} // namespace neocpp
void Signer::addRule(const SharedPtr<WitnessRule>& rule) {
    if (rules_.size() >= NeoConstants::MAX_SIGNER_SUBITEMS) {
        throw TransactionException("Maximum number of rules exceeded");
    }
    rules_.push_back(rule);
} // namespace neocpp
bool Signer::isGlobal() const {
    return scopes_ == WitnessScope::GLOBAL;
} // namespace neocpp
bool Signer::hasScope(WitnessScope scope) const {
    return (static_cast<uint8_t>(scopes_) & static_cast<uint8_t>(scope)) != 0;
} // namespace neocpp
size_t Signer::getSize() const {
    size_t size = NeoConstants::HASH160_SIZE + 1; // account + scopes

    if (hasScope(WitnessScope::CUSTOM_CONTRACTS)) {
        size += 1 + allowedContracts_.size() * NeoConstants::HASH160_SIZE;
    }

    if (hasScope(WitnessScope::CUSTOM_GROUPS)) {
        size += 1 + allowedGroups_.size() * 33; // Assuming compressed public keys
    }

    if (hasScope(WitnessScope::WITNESS_RULES)) {
        size += 1;
        for (const auto& rule : rules_) {
            size += rule->getSize();
        }
    }

    return size;
} // namespace neocpp
void Signer::serialize(BinaryWriter& writer) const {
    account_.serialize(writer);
    writer.writeUInt8(static_cast<uint8_t>(scopes_));

    if (hasScope(WitnessScope::CUSTOM_CONTRACTS)) {
        writer.writeVarInt(allowedContracts_.size());
        for (const auto& contract : allowedContracts_) {
            contract.serialize(writer);
        }
    }

    if (hasScope(WitnessScope::CUSTOM_GROUPS)) {
        writer.writeVarInt(allowedGroups_.size());
        for (const auto& group : allowedGroups_) {
            writer.writeBytes(group);
        }
    }

    if (hasScope(WitnessScope::WITNESS_RULES)) {
        writer.writeVarInt(rules_.size());
        for (const auto& rule : rules_) {
            rule->serialize(writer);
        }
    }
} // namespace neocpp
SharedPtr<Signer> Signer::deserialize(BinaryReader& reader) {
    Hash160 account = Hash160::deserialize(reader);
    WitnessScope scopes = WitnessScopeHelper::fromByte(reader.readUInt8());

    auto signer = std::make_shared<Signer>(account, scopes);

    if (signer->hasScope(WitnessScope::CUSTOM_CONTRACTS)) {
        uint64_t count = reader.readVarInt();
        for (uint64_t i = 0; i < count; ++i) {
            signer->allowedContracts_.push_back(Hash160::deserialize(reader));
        }
    }

    if (signer->hasScope(WitnessScope::CUSTOM_GROUPS)) {
        uint64_t count = reader.readVarInt();
        for (uint64_t i = 0; i < count; ++i) {
            signer->allowedGroups_.push_back(reader.readBytes(33));
        }
    }

    if (signer->hasScope(WitnessScope::WITNESS_RULES)) {
        uint64_t count = reader.readVarInt();
        for (uint64_t i = 0; i < count; ++i) {
            signer->rules_.push_back(WitnessRule::deserialize(reader));
        }
    }

    return signer;
} // namespace neocpp
bool Signer::operator==(const Signer& other) const {
    return account_ == other.account_ &&
           scopes_ == other.scopes_ &&
           allowedContracts_ == other.allowedContracts_ &&
           allowedGroups_ == other.allowedGroups_;
} // namespace neocpp
bool Signer::operator!=(const Signer& other) const {
    return !(*this == other);
} // namespace neocpp
nlohmann::json Signer::toJson() const {
    nlohmann::json json;
    json["account"] = account_.toString();
    json["scopes"] = WitnessScopeHelper::toJsonString(scopes_);

    if (!allowedContracts_.empty()) {
        nlohmann::json contracts = nlohmann::json::array();
        for (const auto& contract : allowedContracts_) {
            contracts.push_back(contract.toString());
        }
        json["allowedcontracts"] = contracts;
    }

    if (!allowedGroups_.empty()) {
        nlohmann::json groups = nlohmann::json::array();
        for (const auto& group : allowedGroups_) {
            groups.push_back(Hex::encode(group));
        }
        json["allowedgroups"] = groups;
    }

    if (!rules_.empty()) {
        nlohmann::json rulesJson = nlohmann::json::array();
        for (const auto& rule : rules_) {
            if (rule) {
                rulesJson.push_back(rule->toJson());
            }
        }
        json["rules"] = rulesJson;
    }

    return json;
} // namespace neocpp
} // namespace neocpp
