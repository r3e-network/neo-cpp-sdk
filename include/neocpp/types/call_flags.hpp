#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

namespace neocpp {

/// Call flags for contract invocations
enum class CallFlags : uint8_t {
    NONE = 0x00,
    READ_STATES = 0b00000001,
    WRITE_STATES = 0b00000010,
    ALLOW_CALL = 0b00000100,
    ALLOW_NOTIFY = 0b00001000,
    STATES = READ_STATES | WRITE_STATES,
    READ_ONLY = READ_STATES | ALLOW_CALL,
    ALL = STATES | ALLOW_CALL | ALLOW_NOTIFY
};

/// Helper class for CallFlags operations
class CallFlagsHelper {
public:
    /// Convert enum to byte value
    static uint8_t toByte(CallFlags flags) {
        return static_cast<uint8_t>(flags);
    }

    /// Convert byte value to enum
    static CallFlags fromByte(uint8_t value) {
        switch (value) {
            case 0x00: return CallFlags::NONE;
            case 0x01: return CallFlags::READ_STATES;
            case 0x02: return CallFlags::WRITE_STATES;
            case 0x04: return CallFlags::ALLOW_CALL;
            case 0x08: return CallFlags::ALLOW_NOTIFY;
            case 0x03: return CallFlags::STATES;
            case 0x05: return CallFlags::READ_ONLY;
            case 0x0F: return CallFlags::ALL;
            default:
                throw std::invalid_argument("There exists no call flag with the provided byte value: " + std::to_string(value));
        }
    }

    /// Combine multiple call flags
    static CallFlags combine(CallFlags a, CallFlags b) {
        return static_cast<CallFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    /// Check if a flag is set
    static bool hasFlag(CallFlags flags, CallFlags flag) {
        return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(flag)) != 0;
    }
};

/// Bitwise OR operator for CallFlags
inline CallFlags operator|(CallFlags a, CallFlags b) {
    return static_cast<CallFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
} // namespace neocpp
/// Bitwise AND operator for CallFlags
inline CallFlags operator&(CallFlags a, CallFlags b) {
    return static_cast<CallFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
} // namespace neocpp
} // namespace neocpp
