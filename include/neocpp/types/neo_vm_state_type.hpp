#pragma once

#include <string>
#include <stdexcept>

namespace neocpp {

/// Neo VM state types
enum class NeoVMStateType {
    NONE = 0,
    HALT = 1,
    FAULT = 1 << 1,
    BREAK = 1 << 2
};

/// Helper class for NeoVMStateType operations
class NeoVMStateTypeHelper {
public:
    /// Convert enum to integer value
    static int toInt(NeoVMStateType state) {
        return static_cast<int>(state);
    }

    /// Convert integer value to enum
    static NeoVMStateType fromInt(int value) {
        switch (value) {
            case 0: return NeoVMStateType::NONE;
            case 1: return NeoVMStateType::HALT;
            case 2: return NeoVMStateType::FAULT;
            case 4: return NeoVMStateType::BREAK;
            default:
                throw std::invalid_argument("Unknown NeoVMStateType value: " + std::to_string(value));
        }
    }

    /// Convert enum to JSON string value
    static std::string toJsonString(NeoVMStateType state) {
        switch (state) {
            case NeoVMStateType::NONE: return "NONE";
            case NeoVMStateType::HALT: return "HALT";
            case NeoVMStateType::FAULT: return "FAULT";
            case NeoVMStateType::BREAK: return "BREAK";
            default:
                throw std::invalid_argument("Unknown NeoVMStateType");
        }
    }

    /// Convert JSON string value to enum
    static NeoVMStateType fromJsonString(const std::string& value) {
        if (value == "NONE" || value.empty()) return NeoVMStateType::NONE;
        if (value == "HALT") return NeoVMStateType::HALT;
        if (value == "FAULT") return NeoVMStateType::FAULT;
        if (value == "BREAK") return NeoVMStateType::BREAK;
        throw std::invalid_argument("Unknown NeoVMStateType string: " + value);
    }
};

} // namespace neocpp
