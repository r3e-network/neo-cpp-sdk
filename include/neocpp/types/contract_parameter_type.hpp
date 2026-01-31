#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace neocpp {

/// Enumeration of contract parameter types
enum class ContractParameterType : uint8_t {
    ANY = 0x00,
    BOOLEAN = 0x10,
    INTEGER = 0x11,
    BYTE_ARRAY = 0x12,
    STRING = 0x13,
    HASH160 = 0x14,
    HASH256 = 0x15,
    PUBLIC_KEY = 0x16,
    SIGNATURE = 0x17,
    ARRAY = 0x20,
    MAP = 0x22,
    INTEROP_INTERFACE = 0x30,
    VOID = 0xff
};

/// Helper class for ContractParameterType operations
class ContractParameterTypeHelper {
public:
    /// Convert enum to byte value
    static uint8_t toByte(ContractParameterType type) {
        return static_cast<uint8_t>(type);
    }

    /// Convert byte value to enum
    static ContractParameterType fromByte(uint8_t value) {
        switch (value) {
            case 0x00: return ContractParameterType::ANY;
            case 0x10: return ContractParameterType::BOOLEAN;
            case 0x11: return ContractParameterType::INTEGER;
            case 0x12: return ContractParameterType::BYTE_ARRAY;
            case 0x13: return ContractParameterType::STRING;
            case 0x14: return ContractParameterType::HASH160;
            case 0x15: return ContractParameterType::HASH256;
            case 0x16: return ContractParameterType::PUBLIC_KEY;
            case 0x17: return ContractParameterType::SIGNATURE;
            case 0x20: return ContractParameterType::ARRAY;
            case 0x22: return ContractParameterType::MAP;
            case 0x30: return ContractParameterType::INTEROP_INTERFACE;
            case 0xff: return ContractParameterType::VOID;
            default:
                throw std::invalid_argument("Unknown ContractParameterType value: " + std::to_string(value));
        }
    }

    /// Convert enum to JSON string value
    static std::string toJsonString(ContractParameterType type) {
        switch (type) {
            case ContractParameterType::ANY: return "Any";
            case ContractParameterType::BOOLEAN: return "Boolean";
            case ContractParameterType::INTEGER: return "Integer";
            case ContractParameterType::BYTE_ARRAY: return "ByteArray";
            case ContractParameterType::STRING: return "String";
            case ContractParameterType::HASH160: return "Hash160";
            case ContractParameterType::HASH256: return "Hash256";
            case ContractParameterType::PUBLIC_KEY: return "PublicKey";
            case ContractParameterType::SIGNATURE: return "Signature";
            case ContractParameterType::ARRAY: return "Array";
            case ContractParameterType::MAP: return "Map";
            case ContractParameterType::INTEROP_INTERFACE: return "InteropInterface";
            case ContractParameterType::VOID: return "Void";
            default:
                throw std::invalid_argument("Unknown ContractParameterType");
        }
    }

    /// Convert JSON string value to enum
    static ContractParameterType fromJsonString(const std::string& value) {
        if (value == "Any") return ContractParameterType::ANY;
        if (value == "Boolean") return ContractParameterType::BOOLEAN;
        if (value == "Integer") return ContractParameterType::INTEGER;
        if (value == "ByteArray") return ContractParameterType::BYTE_ARRAY;
        if (value == "String") return ContractParameterType::STRING;
        if (value == "Hash160") return ContractParameterType::HASH160;
        if (value == "Hash256") return ContractParameterType::HASH256;
        if (value == "PublicKey") return ContractParameterType::PUBLIC_KEY;
        if (value == "Signature") return ContractParameterType::SIGNATURE;
        if (value == "Array") return ContractParameterType::ARRAY;
        if (value == "Map") return ContractParameterType::MAP;
        if (value == "InteropInterface") return ContractParameterType::INTEROP_INTERFACE;
        if (value == "Void") return ContractParameterType::VOID;
        throw std::invalid_argument("Unknown ContractParameterType string: " + value);
    }
};

} // namespace neocpp
