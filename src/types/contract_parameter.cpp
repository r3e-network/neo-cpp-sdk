
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/utils/hex.hpp"

namespace neocpp {

// Default constructor
ContractParameter::ContractParameter() : type_(ContractParameterType::ANY), value_(std::monostate{}) {
}

// Constructor with type only
ContractParameter::ContractParameter(ContractParameterType type) : type_(type), value_(std::monostate{}) {
}

// Constructor for bool
ContractParameter::ContractParameter(bool value) : type_(ContractParameterType::BOOLEAN), value_(value) {
}

// Constructor for int64_t
ContractParameter::ContractParameter(int64_t value) : type_(ContractParameterType::INTEGER), value_(value) {
}

// Constructor for Bytes
ContractParameter::ContractParameter(const Bytes& value) : type_(ContractParameterType::BYTE_ARRAY), value_(value) {
}

// Constructor for string
ContractParameter::ContractParameter(const std::string& value) : type_(ContractParameterType::STRING), value_(value) {
}

// Constructor for C-string
ContractParameter::ContractParameter(const char* value) 
    : type_(ContractParameterType::STRING), value_(std::string(value)) {
}

// Constructor for array
ContractParameter::ContractParameter(const std::vector<ContractParameter>& value) 
    : type_(ContractParameterType::ARRAY), value_(value) {
}

// Constructor for map
ContractParameter::ContractParameter(const std::map<ContractParameter, ContractParameter>& value) 
    : type_(ContractParameterType::MAP), value_(value) {
}

// Constructor for Hash160
ContractParameter::ContractParameter(const Hash160& value) 
    : type_(ContractParameterType::HASH160), value_(value) {
}

// Constructor for Hash256
ContractParameter::ContractParameter(const Hash256& value) 
    : type_(ContractParameterType::HASH256), value_(value) {
}

// Constructor for ECPublicKey
ContractParameter::ContractParameter(const SharedPtr<ECPublicKey>& value) 
    : type_(ContractParameterType::PUBLIC_KEY), value_(value) {
} // namespace neocpp
ContractParameter ContractParameter::boolean(bool value) {
    ContractParameter param(ContractParameterType::BOOLEAN);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::integer(int64_t value) {
    ContractParameter param(ContractParameterType::INTEGER);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::byteArray(const Bytes& value) {
    ContractParameter param(ContractParameterType::BYTE_ARRAY);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::string(const std::string& value) {
    ContractParameter param(ContractParameterType::STRING);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::hash160(const Hash160& value) {
    ContractParameter param(ContractParameterType::HASH160);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::hash256(const Hash256& value) {
    ContractParameter param(ContractParameterType::HASH256);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::publicKey(const SharedPtr<ECPublicKey>& value) {
    ContractParameter param(ContractParameterType::PUBLIC_KEY);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::signature(const Bytes& value) {
    ContractParameter param(ContractParameterType::SIGNATURE);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::array(const std::vector<ContractParameter>& value) {
    ContractParameter param(ContractParameterType::ARRAY);
    param.value_ = value;
    return param;
} // namespace neocpp
ContractParameter ContractParameter::map(const std::map<ContractParameter, ContractParameter>& value) {
    ContractParameter param(ContractParameterType::MAP);
    param.value_ = value;
    return param;
} // namespace neocpp
bool ContractParameter::getBoolean() const {
    if (type_ != ContractParameterType::BOOLEAN) {
        throw IllegalArgumentException("Parameter is not a boolean");
    }
    return std::get<bool>(value_);
} // namespace neocpp
int64_t ContractParameter::getInteger() const {
    if (type_ != ContractParameterType::INTEGER) {
        throw IllegalArgumentException("Parameter is not an integer");
    }
    return std::get<int64_t>(value_);
} // namespace neocpp
Bytes ContractParameter::getByteArray() const {
    if (type_ != ContractParameterType::BYTE_ARRAY && type_ != ContractParameterType::SIGNATURE) {
        throw IllegalArgumentException("Parameter is not a byte array");
    }
    return std::get<Bytes>(value_);
} // namespace neocpp
std::string ContractParameter::getString() const {
    if (type_ != ContractParameterType::STRING) {
        throw IllegalArgumentException("Parameter is not a string");
    }
    return std::get<std::string>(value_);
} // namespace neocpp
Hash160 ContractParameter::getHash160() const {
    if (type_ != ContractParameterType::HASH160) {
        throw IllegalArgumentException("Parameter is not a Hash160");
    }
    return std::get<Hash160>(value_);
} // namespace neocpp
Hash256 ContractParameter::getHash256() const {
    if (type_ != ContractParameterType::HASH256) {
        throw IllegalArgumentException("Parameter is not a Hash256");
    }
    return std::get<Hash256>(value_);
} // namespace neocpp
SharedPtr<ECPublicKey> ContractParameter::getPublicKey() const {
    if (type_ != ContractParameterType::PUBLIC_KEY) {
        throw IllegalArgumentException("Parameter is not a public key");
    }
    return std::get<SharedPtr<ECPublicKey>>(value_);
} // namespace neocpp
std::vector<ContractParameter> ContractParameter::getArray() const {
    if (type_ != ContractParameterType::ARRAY) {
        throw IllegalArgumentException("Parameter is not an array");
    }
    return std::get<std::vector<ContractParameter>>(value_);
} // namespace neocpp
std::map<ContractParameter, ContractParameter> ContractParameter::getMap() const {
    if (type_ != ContractParameterType::MAP) {
        throw IllegalArgumentException("Parameter is not a map");
    }
    return std::get<std::map<ContractParameter, ContractParameter>>(value_);
} // namespace neocpp
bool ContractParameter::operator<(const ContractParameter& other) const {
    if (type_ != other.type_) {
        return type_ < other.type_;
    }
    return value_ < other.value_;
} // namespace neocpp
bool ContractParameter::operator==(const ContractParameter& other) const {
    return type_ == other.type_ && value_ == other.value_;
}

bool ContractParameter::operator!=(const ContractParameter& other) const {
    return !(*this == other);
}

ContractParameter ContractParameter::any() {
    return ContractParameter(ContractParameterType::ANY);
}

ContractParameter ContractParameter::voidParam() {
    return ContractParameter(ContractParameterType::VOID);
}

bool ContractParameter::isNull() const {
    return std::holds_alternative<std::monostate>(value_);
}

nlohmann::json ContractParameter::toRpcJson() const {
    nlohmann::json result;
    result["type"] = ContractParameterTypeHelper::toJsonString(type_);
    
    switch (type_) {
        case ContractParameterType::BOOLEAN:
            result["value"] = getBoolean();
            break;
        case ContractParameterType::INTEGER:
            result["value"] = std::to_string(getInteger());
            break;
        case ContractParameterType::BYTE_ARRAY:
        case ContractParameterType::SIGNATURE:
            result["value"] = Hex::encode(getByteArray(), false);
            break;
        case ContractParameterType::STRING:
            result["value"] = getString();
            break;
        case ContractParameterType::HASH160:
            result["value"] = getHash160().toString();
            break;
        case ContractParameterType::HASH256:
            result["value"] = getHash256().toString();
            break;
        case ContractParameterType::PUBLIC_KEY: {
            auto pk = getPublicKey();
            if (pk) {
                result["value"] = Hex::encode(pk->getEncoded(), false);
            }
            break;
        }
        case ContractParameterType::ARRAY: {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : getArray()) {
                arr.push_back(item.toRpcJson());
            }
            result["value"] = arr;
            break;
        }
        case ContractParameterType::MAP: {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& [key, value] : getMap()) {
                nlohmann::json entry;
                entry["key"] = key.toRpcJson();
                entry["value"] = value.toRpcJson();
                arr.push_back(entry);
            }
            result["value"] = arr;
            break;
        }
        case ContractParameterType::ANY:
        case ContractParameterType::VOID:
        case ContractParameterType::INTEROP_INTERFACE:
        default:
            // These types have no value field
            break;
    }
    return result;
}

ContractParameter ContractParameter::fromRpcJson(const nlohmann::json& json) {
    if (!json.contains("type")) {
        throw IllegalArgumentException("JSON missing 'type' field");
    }
    
    ContractParameterType type = ContractParameterTypeHelper::fromJsonString(json["type"].get<std::string>());
    
    // Handle types without value
    if (!json.contains("value") || json["value"].is_null()) {
        return ContractParameter(type);
    }
    
    switch (type) {
        case ContractParameterType::BOOLEAN:
            return ContractParameter::boolean(json["value"].get<bool>());
        case ContractParameterType::INTEGER: {
            std::string intStr = json["value"].get<std::string>();
            return ContractParameter::integer(std::stoll(intStr));
        }
        case ContractParameterType::BYTE_ARRAY:
        case ContractParameterType::SIGNATURE:
            return ContractParameter::byteArray(Hex::decode(json["value"].get<std::string>()));
        case ContractParameterType::STRING:
            return ContractParameter::string(json["value"].get<std::string>());
        case ContractParameterType::HASH160:
            return ContractParameter::hash160(Hash160(json["value"].get<std::string>()));
        case ContractParameterType::HASH256:
            return ContractParameter::hash256(Hash256(json["value"].get<std::string>()));
        case ContractParameterType::PUBLIC_KEY:
            // Public key from hex - would need ECPublicKey implementation
            return ContractParameter::byteArray(Hex::decode(json["value"].get<std::string>()));
        case ContractParameterType::ARRAY: {
            std::vector<ContractParameter> arr;
            for (const auto& item : json["value"]) {
                arr.push_back(fromRpcJson(item));
            }
            return ContractParameter::array(arr);
        }
        case ContractParameterType::MAP: {
            std::map<ContractParameter, ContractParameter> map;
            for (const auto& entry : json["value"]) {
                ContractParameter key = fromRpcJson(entry["key"]);
                ContractParameter value = fromRpcJson(entry["value"]);
                map[key] = value;
            }
            return ContractParameter::map(map);
        }
        default:
            return ContractParameter(type);
    }
}

nlohmann::json ContractParameter::toJson() const {
    // For now, delegate to toRpcJson - they have the same format
    return toRpcJson();
}

ContractParameter ContractParameter::fromJson(const nlohmann::json& json) {
    // For now, delegate to fromRpcJson - they have the same format
    return fromRpcJson(json);
}

} // namespace neocpp
