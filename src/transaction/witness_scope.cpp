#include "neocpp/transaction/witness_scope.hpp"
#include "neocpp/exceptions.hpp"
#include <algorithm>
#include <sstream>

namespace neocpp {

std::string witnessScope2String(WitnessScope scope) {
    switch (scope) {
        case WitnessScope::NONE:
            return "None";
        case WitnessScope::CALLED_BY_ENTRY:
            return "CalledByEntry";
        case WitnessScope::CUSTOM_CONTRACTS:
            return "CustomContracts";
        case WitnessScope::CUSTOM_GROUPS:
            return "CustomGroups";
        case WitnessScope::WITNESS_RULES:
            return "WitnessRules";
        case WitnessScope::GLOBAL:
            return "Global";
        default:
            throw IllegalArgumentException("Unknown witness scope");
    }
}

WitnessScope string2WitnessScope(const std::string& scopeStr) {
    if (scopeStr == "None") {
        return WitnessScope::NONE;
    } else if (scopeStr == "CalledByEntry") {
        return WitnessScope::CALLED_BY_ENTRY;
    } else if (scopeStr == "CustomContracts") {
        return WitnessScope::CUSTOM_CONTRACTS;
    } else if (scopeStr == "CustomGroups") {
        return WitnessScope::CUSTOM_GROUPS;
    } else if (scopeStr == "WitnessRules") {
        return WitnessScope::WITNESS_RULES;
    } else if (scopeStr == "Global") {
        return WitnessScope::GLOBAL;
    } else {
        throw IllegalArgumentException("Unknown witness scope string: " + scopeStr);
    }
}

uint8_t combineScopes(const std::vector<WitnessScope>& scopes) {
    uint8_t result = 0;
    for (const auto& scope : scopes) {
        result |= static_cast<uint8_t>(scope);
    }
    return result;
}

std::vector<WitnessScope> extractScopes(uint8_t combinedScope) {
    std::vector<WitnessScope> result;

    // Check for global scope which cannot be combined
    if (combinedScope & static_cast<uint8_t>(WitnessScope::GLOBAL)) {
        result.push_back(WitnessScope::GLOBAL);
        return result;
    }

    // Check for each scope bit
    if (combinedScope & static_cast<uint8_t>(WitnessScope::NONE)) {
        result.push_back(WitnessScope::NONE);
    }
    if (combinedScope & static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY)) {
        result.push_back(WitnessScope::CALLED_BY_ENTRY);
    }
    if (combinedScope & static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS)) {
        result.push_back(WitnessScope::CUSTOM_CONTRACTS);
    }
    if (combinedScope & static_cast<uint8_t>(WitnessScope::CUSTOM_GROUPS)) {
        result.push_back(WitnessScope::CUSTOM_GROUPS);
    }
    if (combinedScope & static_cast<uint8_t>(WitnessScope::WITNESS_RULES)) {
        result.push_back(WitnessScope::WITNESS_RULES);
    }

    return result;
}

std::string formatScopeSet(const std::vector<WitnessScope>& scopes) {
    if (scopes.empty()) {
        return "[]";
    }

    std::stringstream ss;
    ss << "[";

    for (size_t i = 0; i < scopes.size(); ++i) {
        ss << witnessScope2String(scopes[i]);
        if (i < scopes.size() - 1) {
            ss << ", ";
        }
    }

    ss << "]";
    return ss.str();
}

} // namespace neocpp
