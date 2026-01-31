#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/contract_parameter_type.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/hash256.hpp"

namespace neocpp {

// Forward declaration
class ECPublicKey;

/// Represents a parameter for contract invocation
class ContractParameter {
public:
    using ValueType = std::variant<
        std::monostate,           // Any or Void
        bool,                     // Boolean
        int64_t,                  // Integer
        Bytes,                    // ByteArray
        std::string,              // String
        Hash160,                  // Hash160
        Hash256,                  // Hash256
        SharedPtr<ECPublicKey>,   // PublicKey
        std::vector<ContractParameter>, // Array
        std::map<ContractParameter, ContractParameter> // Map
    >;

private:
    ContractParameterType type_;
    ValueType value_;

public:
    /// Default constructor (creates Any type)
    ContractParameter();

    /// Constructor with type
    explicit ContractParameter(ContractParameterType type);

    /// Constructor for boolean
    explicit ContractParameter(bool value);

    /// Constructor for integer
    explicit ContractParameter(int64_t value);

    /// Constructor for byte array
    explicit ContractParameter(const Bytes& value);

    /// Constructor for string
    explicit ContractParameter(const std::string& value);
    
    /// Constructor for C-string (handles string literals)
    explicit ContractParameter(const char* value);

    /// Constructor for Hash160
    explicit ContractParameter(const Hash160& value);

    /// Constructor for Hash256
    explicit ContractParameter(const Hash256& value);

    /// Constructor for public key
    explicit ContractParameter(const SharedPtr<ECPublicKey>& value);

    /// Constructor for array
    explicit ContractParameter(const std::vector<ContractParameter>& value);

    /// Constructor for map
    explicit ContractParameter(const std::map<ContractParameter, ContractParameter>& value);

    /// Get the parameter type
    [[nodiscard]] ContractParameterType getType() const { return type_; }

    /// Get the parameter value
    const ValueType& getValue() const { return value_; }

    /// Set the parameter type
    void setType(ContractParameterType type) { type_ = type; }

    /// Set the parameter value
    void setValue(const ValueType& value) { value_ = value; }

    // Value getters with type checking
    [[nodiscard]] bool getBoolean() const;
    [[nodiscard]] int64_t getInteger() const;
    [[nodiscard]] Bytes getByteArray() const;
    std::string getString() const;
    [[nodiscard]] Hash160 getHash160() const;
    [[nodiscard]] Hash256 getHash256() const;
    SharedPtr<ECPublicKey> getPublicKey() const;
    std::vector<ContractParameter> getArray() const;
    std::map<ContractParameter, ContractParameter> getMap() const;

    /// Check if parameter is null/void
    bool isNull() const;

    /// Convert to JSON representation
    nlohmann::json toJson() const;

    /// Create from JSON representation
    static ContractParameter fromJson(const nlohmann::json& json);

    /// Convert to RPC parameter format
    nlohmann::json toRpcJson() const;

    /// Create from RPC parameter format
    static ContractParameter fromRpcJson(const nlohmann::json& json);

    // Comparison operators (needed for map key)
    bool operator<(const ContractParameter& other) const;
    bool operator==(const ContractParameter& other) const;
    bool operator!=(const ContractParameter& other) const;

    // Static factory methods for convenience
    static ContractParameter any();
    static ContractParameter boolean(bool value);
    static ContractParameter integer(int64_t value);
    static ContractParameter byteArray(const Bytes& value);
    static ContractParameter string(const std::string& value);
    static ContractParameter hash160(const Hash160& value);
    static ContractParameter hash256(const Hash256& value);
    static ContractParameter publicKey(const SharedPtr<ECPublicKey>& value);
    static ContractParameter array(const std::vector<ContractParameter>& value);
    static ContractParameter map(const std::map<ContractParameter, ContractParameter>& value);
    static ContractParameter signature(const Bytes& value);
    static ContractParameter voidParam();
};

} // namespace neocpp
