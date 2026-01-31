#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

namespace neocpp {

/// Witness scope enumeration for transaction signers
enum class WitnessScope : uint8_t {
    /// A witness with this scope is only used for transactions and is disabled in contracts.
    NONE = 0x00,

    /// This scope limits the use of a witness to the level of the contract called in the transaction.
    /// I.e. it only allows the invoked contract to use the witness.
    CALLED_BY_ENTRY = 0x01,

    /// This scope allows the specification of additional contracts in which the witness can be used.
    CUSTOM_CONTRACTS = 0x10,

    /// This scope allows the specification of contract groups in which the witness can be used.
    CUSTOM_GROUPS = 0x20,

    /// Indicates that the current context must satisfy the specified rules.
    WITNESS_RULES = 0x40,

    /// The global scope allows to use a witness in all contexts. It cannot be combined with other scopes.
    GLOBAL = 0x80
};

/// Helper class for WitnessScope operations
class WitnessScopeHelper {
public:
    /// Convert enum to byte value
    static uint8_t toByte(WitnessScope scope) {
        return static_cast<uint8_t>(scope);
    }

    /// Convert byte value to enum
    static WitnessScope fromByte(uint8_t value) {
        switch (value) {
            case 0x00: return WitnessScope::NONE;
            case 0x01: return WitnessScope::CALLED_BY_ENTRY;
            case 0x10: return WitnessScope::CUSTOM_CONTRACTS;
            case 0x20: return WitnessScope::CUSTOM_GROUPS;
            case 0x40: return WitnessScope::WITNESS_RULES;
            case 0x80: return WitnessScope::GLOBAL;
            default:
                // Handle combined scopes - validate that only valid bits are set
                // Valid bits: 0x01 (CALLED_BY_ENTRY), 0x10 (CUSTOM_CONTRACTS), 0x20 (CUSTOM_GROUPS), 
                //             0x40 (WITNESS_RULES), 0x80 (GLOBAL)
                // Combined valid mask: 0xF1 = 0x01 | 0x10 | 0x20 | 0x40 | 0x80
                if ((value & 0xF1) == value) {
                    // Note: 0x01 (CALLED_BY_ENTRY) and 0x80 (GLOBAL) are mutually exclusive
                    if ((value & 0x01) != 0 && (value & 0x80) != 0) {
                        throw std::invalid_argument("CALLED_BY_ENTRY and GLOBAL are mutually exclusive");
                    }
                    return static_cast<WitnessScope>(value);
                }
                throw std::invalid_argument("Unknown WitnessScope value: " + std::to_string(value));
        }
    }

    /// Convert enum to JSON string value
    /// Handles combined scopes by returning comma-separated values
    static std::string toJsonString(WitnessScope scope) {
        uint8_t scopeByte = static_cast<uint8_t>(scope);
        
        // Handle simple cases first
        switch (scope) {
            case WitnessScope::NONE: return "None";
            case WitnessScope::CALLED_BY_ENTRY: return "CalledByEntry";
            case WitnessScope::CUSTOM_CONTRACTS: return "CustomContracts";
            case WitnessScope::CUSTOM_GROUPS: return "CustomGroups";
            case WitnessScope::WITNESS_RULES: return "WitnessRules";
            case WitnessScope::GLOBAL: return "Global";
            default:
                break;  // Combined scope, handle below
        }
        
        // Handle combined scopes
        std::vector<std::string> parts;
        if ((scopeByte & 0x01) != 0) parts.push_back("CalledByEntry");
        if ((scopeByte & 0x10) != 0) parts.push_back("CustomContracts");
        if ((scopeByte & 0x20) != 0) parts.push_back("CustomGroups");
        if ((scopeByte & 0x40) != 0) parts.push_back("WitnessRules");
        if ((scopeByte & 0x80) != 0) parts.push_back("Global");
        
        if (parts.empty()) {
            throw std::invalid_argument("Unknown WitnessScope");
        }
        
        // Join with comma
        std::string result = parts[0];
        for (size_t i = 1; i < parts.size(); ++i) {
            result += ", " + parts[i];
        }
        return result;
    }

    /// Convert enum to JSON array of strings
    /// Handles combined scopes by returning an array of values
    static std::vector<std::string> toJsonArray(WitnessScope scope) {
        auto scopes = extractCombinedScopes(static_cast<uint8_t>(scope));
        std::vector<std::string> result;
        result.reserve(scopes.size());
        for (const auto& entry : scopes) {
            result.push_back(toJsonString(entry));
        }
        return result;
    }

    /// Convert JSON string value to enum
    static WitnessScope fromJsonString(const std::string& value) {
        // Check for simple cases first
        if (value == "None") return WitnessScope::NONE;
        if (value == "CalledByEntry") return WitnessScope::CALLED_BY_ENTRY;
        if (value == "CustomContracts") return WitnessScope::CUSTOM_CONTRACTS;
        if (value == "CustomGroups") return WitnessScope::CUSTOM_GROUPS;
        if (value == "WitnessRules") return WitnessScope::WITNESS_RULES;
        if (value == "Global") return WitnessScope::GLOBAL;
        
        // Handle combined scopes (comma-separated)
        if (value.find(',') != std::string::npos) {
            uint8_t combined = 0;
            size_t start = 0;
            size_t end = value.find(',');
            
            while (end != std::string::npos) {
                std::string part = value.substr(start, end - start);
                // Trim whitespace
                size_t wsStart = part.find_first_not_of(" \t");
                size_t wsEnd = part.find_last_not_of(" \t");
                if (wsStart != std::string::npos && wsEnd != std::string::npos) {
                    part = part.substr(wsStart, wsEnd - wsStart + 1);
                }
                combined |= toByte(fromJsonString(part));
                start = end + 1;
                end = value.find(',', start);
            }
            
            // Handle last part
            std::string part = value.substr(start);
            size_t wsStart = part.find_first_not_of(" \t");
            size_t wsEnd = part.find_last_not_of(" \t");
            if (wsStart != std::string::npos && wsEnd != std::string::npos) {
                part = part.substr(wsStart, wsEnd - wsStart + 1);
            }
            combined |= toByte(fromJsonString(part));
            
            return fromByte(combined);
        }
        
        throw std::invalid_argument("Unknown WitnessScope string: " + value);
    }

    /// Encodes the given scopes in one byte.
    /// @param scopes The scopes to encode
    /// @return The scope encoding byte
    static uint8_t combineScopes(const std::vector<WitnessScope>& scopes) {
        uint8_t result = 0;
        for (const auto& scope : scopes) {
            result |= toByte(scope);
        }
        return result;
    }

    /// Extracts the scopes encoded in the given byte.
    /// @param combinedScopes The byte representation of the scopes
    /// @return The list of scopes encoded by the given byte
    static std::vector<WitnessScope> extractCombinedScopes(uint8_t combinedScopes) {
        std::vector<WitnessScope> result;

        if (combinedScopes == 0x00) {
            result.push_back(WitnessScope::NONE);
            return result;
        }

        // Check each scope bit
        if ((combinedScopes & 0x01) != 0) result.push_back(WitnessScope::CALLED_BY_ENTRY);
        if ((combinedScopes & 0x10) != 0) result.push_back(WitnessScope::CUSTOM_CONTRACTS);
        if ((combinedScopes & 0x20) != 0) result.push_back(WitnessScope::CUSTOM_GROUPS);
        if ((combinedScopes & 0x40) != 0) result.push_back(WitnessScope::WITNESS_RULES);
        if ((combinedScopes & 0x80) != 0) result.push_back(WitnessScope::GLOBAL);

        return result;
    }
};

} // namespace neocpp
