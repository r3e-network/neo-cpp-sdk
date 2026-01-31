#pragma once

#include <cstdint>
#include <string>

namespace neocpp {

/// Witness rule action
enum class WitnessAction : uint8_t {
    /// Deny the witness
    DENY = 0x00,

    /// Allow the witness
    ALLOW = 0x01
};

/// Convert WitnessAction to string
inline std::string toString(WitnessAction action) {
    switch (action) {
        case WitnessAction::DENY:
            return "Deny";
        case WitnessAction::ALLOW:
            return "Allow";
        default:
            return "Unknown";
    }
} // namespace neocpp
/// Convert string to WitnessAction
inline WitnessAction witnessActionFromString(const std::string& str) {
    if (str == "Deny" || str == "deny" || str == "DENY") {
        return WitnessAction::DENY;
    } else if (str == "Allow" || str == "allow" || str == "ALLOW") {
        return WitnessAction::ALLOW;
    }
    return WitnessAction::DENY;
} // namespace neocpp
} // namespace neocpp
