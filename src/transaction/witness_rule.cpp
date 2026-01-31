
#include "neocpp/transaction/witness_rule.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/utils/hex.hpp"

namespace neocpp {

// WitnessCondition implementation

SharedPtr<WitnessCondition> WitnessCondition::boolean(bool value) {
    auto condition = SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::BOOLEAN));
    condition->boolValue_ = value;
    return condition;
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::notCondition(const SharedPtr<WitnessCondition>& condition) {
    auto result = SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::NOT));
    result->conditions_.push_back(condition);
    return result;
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::andCondition(const std::vector<SharedPtr<WitnessCondition>>& conditions) {
    auto result = SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::AND));
    result->conditions_ = conditions;
    return result;
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::orCondition(const std::vector<SharedPtr<WitnessCondition>>& conditions) {
    auto result = SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::OR));
    result->conditions_ = conditions;
    return result;
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::scriptHash(const Hash160& scriptHash) {
    auto condition = SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::SCRIPT_HASH));
    condition->scriptHash_ = scriptHash;
    return condition;
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::calledByEntry() {
    return SharedPtr<WitnessCondition>(new WitnessCondition(WitnessConditionType::CALLED_BY_ENTRY));
} // namespace neocpp
size_t WitnessCondition::getSize() const {
    size_t size = 1; // type byte

    switch (type_) {
        case WitnessConditionType::BOOLEAN:
            size += 1; // boolean value
            break;
        case WitnessConditionType::NOT:
            if (!conditions_.empty()) {
                size += conditions_[0]->getSize();
            }
            break;
        case WitnessConditionType::AND:
        case WitnessConditionType::OR:
            size += 1; // varint size for count (assuming < 253 conditions)
            if (conditions_.size() >= 253) {
                size += 2; // need 3 bytes total for larger counts
            }
            for (const auto& cond : conditions_) {
                size += cond->getSize();
            }
            break;
        case WitnessConditionType::SCRIPT_HASH:
        case WitnessConditionType::CALLED_BY_CONTRACT:
            size += 20; // Hash160
            break;
        case WitnessConditionType::GROUP:
        case WitnessConditionType::CALLED_BY_GROUP:
            size += 33; // ECPoint compressed
            break;
        case WitnessConditionType::CALLED_BY_ENTRY:
            // No additional data
            break;
    }

    return size;
} // namespace neocpp
void WitnessCondition::serialize(BinaryWriter& writer) const {
    writer.writeUInt8(static_cast<uint8_t>(type_));

    switch (type_) {
        case WitnessConditionType::BOOLEAN:
            writer.writeBool(boolValue_);
            break;
        case WitnessConditionType::NOT:
            if (!conditions_.empty()) {
                conditions_[0]->serialize(writer);
            }
            break;
        case WitnessConditionType::AND:
        case WitnessConditionType::OR:
            writer.writeVarInt(conditions_.size());
            for (const auto& cond : conditions_) {
                cond->serialize(writer);
            }
            break;
        case WitnessConditionType::SCRIPT_HASH:
        case WitnessConditionType::CALLED_BY_CONTRACT:
            scriptHash_.serialize(writer);
            break;
        case WitnessConditionType::CALLED_BY_ENTRY:
            // No additional data
            break;
        default:
            throw SerializationException("Unsupported witness condition type");
    }
} // namespace neocpp
SharedPtr<WitnessCondition> WitnessCondition::deserialize(BinaryReader& reader) {
    uint8_t typeByte = reader.readUInt8();
    auto type = static_cast<WitnessConditionType>(typeByte);

    switch (type) {
        case WitnessConditionType::BOOLEAN:
            return boolean(reader.readBool());

        case WitnessConditionType::NOT: {
            auto inner = deserialize(reader);
            return notCondition(inner);
        }

        case WitnessConditionType::AND:
        case WitnessConditionType::OR: {
            uint64_t count = reader.readVarInt();
            std::vector<SharedPtr<WitnessCondition>> conditions;
            for (uint64_t i = 0; i < count; ++i) {
                conditions.push_back(deserialize(reader));
            }
            return type == WitnessConditionType::AND ?
                   andCondition(conditions) : orCondition(conditions);
        }

        case WitnessConditionType::SCRIPT_HASH: {
            Hash160 hash = Hash160::deserialize(reader);
            return scriptHash(hash);
        }

        case WitnessConditionType::CALLED_BY_ENTRY:
            return calledByEntry();

        default:
            throw DeserializationException("Unknown witness condition type: " + std::to_string(typeByte));
    }
} // namespace neocpp
// WitnessRule implementation

size_t WitnessRule::getSize() const {
    size_t size = 1; // action byte
    if (condition_) {
        size += condition_->getSize();
    }
    return size;
} // namespace neocpp
void WitnessRule::serialize(BinaryWriter& writer) const {
    writer.writeUInt8(static_cast<uint8_t>(action_));
    if (condition_) {
        condition_->serialize(writer);
    }
} // namespace neocpp
SharedPtr<WitnessRule> WitnessRule::deserialize(BinaryReader& reader) {
    auto rule = std::make_shared<WitnessRule>();
    rule->action_ = static_cast<WitnessRuleAction>(reader.readUInt8());
    rule->condition_ = WitnessCondition::deserialize(reader);
    return rule;
} // namespace neocpp
nlohmann::json WitnessRule::toJson() const {
    nlohmann::json json;
    json["action"] = action_ == WitnessRuleAction::ALLOW ? "Allow" : "Deny";

    if (condition_) {
        nlohmann::json condJson;

        switch (condition_->getType()) {
            case WitnessConditionType::BOOLEAN:
                condJson["type"] = "Boolean";
                // Note: We would need to add getter methods for condition values
                condJson["value"] = false;
                break;
            case WitnessConditionType::NOT:
                condJson["type"] = "Not";
                condJson["expression"] = nlohmann::json::object();
                break;
            case WitnessConditionType::AND:
                condJson["type"] = "And";
                condJson["expressions"] = nlohmann::json::array();
                break;
            case WitnessConditionType::OR:
                condJson["type"] = "Or";
                condJson["expressions"] = nlohmann::json::array();
                break;
            case WitnessConditionType::SCRIPT_HASH:
                condJson["type"] = "ScriptHash";
                condJson["hash"] = "";  // Would need getter for scriptHash
                break;
            case WitnessConditionType::GROUP:
                condJson["type"] = "Group";
                condJson["publicKey"] = "";
                break;
            case WitnessConditionType::CALLED_BY_ENTRY:
                condJson["type"] = "CalledByEntry";
                break;
            case WitnessConditionType::CALLED_BY_CONTRACT:
                condJson["type"] = "CalledByContract";
                condJson["hash"] = "";
                break;
            case WitnessConditionType::CALLED_BY_GROUP:
                condJson["type"] = "CalledByGroup";
                condJson["publicKey"] = "";
                break;
        }

        json["condition"] = condJson;
    }

    return json;
} // namespace neocpp
} // namespace neocpp
