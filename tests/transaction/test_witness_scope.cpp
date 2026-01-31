#include <catch2/catch_test_macros.hpp>
#include "neocpp/transaction/witness_scope.hpp"
#include <stdexcept>
#include <algorithm>

using namespace neocpp;

TEST_CASE("WitnessScope Tests", "[transaction]") {
    
    SECTION("WitnessScope enum values") {
        REQUIRE(static_cast<uint8_t>(WitnessScope::NONE) == 0x00);
        REQUIRE(static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY) == 0x01);
        REQUIRE(static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS) == 0x10);
        REQUIRE(static_cast<uint8_t>(WitnessScope::CUSTOM_GROUPS) == 0x20);
        REQUIRE(static_cast<uint8_t>(WitnessScope::WITNESS_RULES) == 0x40);
        REQUIRE(static_cast<uint8_t>(WitnessScope::GLOBAL) == 0x80);
    }
    
    SECTION("WitnessScopeHelper toByte conversion") {
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::NONE) == 0x00);
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::CALLED_BY_ENTRY) == 0x01);
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::CUSTOM_CONTRACTS) == 0x10);
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::CUSTOM_GROUPS) == 0x20);
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::WITNESS_RULES) == 0x40);
        REQUIRE(WitnessScopeHelper::toByte(WitnessScope::GLOBAL) == 0x80);
    }
    
    SECTION("WitnessScopeHelper fromByte conversion") {
        REQUIRE(WitnessScopeHelper::fromByte(0x00) == WitnessScope::NONE);
        REQUIRE(WitnessScopeHelper::fromByte(0x01) == WitnessScope::CALLED_BY_ENTRY);
        REQUIRE(WitnessScopeHelper::fromByte(0x10) == WitnessScope::CUSTOM_CONTRACTS);
        REQUIRE(WitnessScopeHelper::fromByte(0x20) == WitnessScope::CUSTOM_GROUPS);
        REQUIRE(WitnessScopeHelper::fromByte(0x40) == WitnessScope::WITNESS_RULES);
        REQUIRE(WitnessScopeHelper::fromByte(0x80) == WitnessScope::GLOBAL);
    }
    
    SECTION("WitnessScopeHelper fromByte with invalid values") {
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromByte(0x02), std::invalid_argument);
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromByte(0x08), std::invalid_argument);
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromByte(0xFF), std::invalid_argument);
    }
    
    SECTION("WitnessScopeHelper JSON string conversion") {
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::NONE) == "None");
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::CALLED_BY_ENTRY) == "CalledByEntry");
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::CUSTOM_CONTRACTS) == "CustomContracts");
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::CUSTOM_GROUPS) == "CustomGroups");
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::WITNESS_RULES) == "WitnessRules");
        REQUIRE(WitnessScopeHelper::toJsonString(WitnessScope::GLOBAL) == "Global");
    }
    
    SECTION("WitnessScopeHelper fromJsonString conversion") {
        REQUIRE(WitnessScopeHelper::fromJsonString("None") == WitnessScope::NONE);
        REQUIRE(WitnessScopeHelper::fromJsonString("CalledByEntry") == WitnessScope::CALLED_BY_ENTRY);
        REQUIRE(WitnessScopeHelper::fromJsonString("CustomContracts") == WitnessScope::CUSTOM_CONTRACTS);
        REQUIRE(WitnessScopeHelper::fromJsonString("CustomGroups") == WitnessScope::CUSTOM_GROUPS);
        REQUIRE(WitnessScopeHelper::fromJsonString("WitnessRules") == WitnessScope::WITNESS_RULES);
        REQUIRE(WitnessScopeHelper::fromJsonString("Global") == WitnessScope::GLOBAL);
    }
    
    SECTION("WitnessScopeHelper fromJsonString with invalid values") {
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromJsonString("Invalid"), std::invalid_argument);
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromJsonString("none"), std::invalid_argument); // Case sensitive
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromJsonString(""), std::invalid_argument);
        REQUIRE_THROWS_AS(WitnessScopeHelper::fromJsonString("Unknown"), std::invalid_argument);
    }
    
    SECTION("WitnessScopeHelper combineScopes") {
        std::vector<WitnessScope> scopes;
        
        // Single scope
        scopes = {WitnessScope::CALLED_BY_ENTRY};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x01);
        
        scopes = {WitnessScope::GLOBAL};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x80);
        
        // Combined scopes
        scopes = {WitnessScope::CALLED_BY_ENTRY, WitnessScope::CUSTOM_CONTRACTS};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x11); // 0x01 | 0x10
        
        scopes = {WitnessScope::CUSTOM_CONTRACTS, WitnessScope::CUSTOM_GROUPS};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x30); // 0x10 | 0x20
        
        scopes = {WitnessScope::CALLED_BY_ENTRY, WitnessScope::CUSTOM_CONTRACTS, WitnessScope::WITNESS_RULES};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x51); // 0x01 | 0x10 | 0x40
        
        // All scopes except NONE and GLOBAL
        scopes = {WitnessScope::CALLED_BY_ENTRY, WitnessScope::CUSTOM_CONTRACTS, 
                 WitnessScope::CUSTOM_GROUPS, WitnessScope::WITNESS_RULES};
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x71); // 0x01 | 0x10 | 0x20 | 0x40
    }
    
    SECTION("WitnessScopeHelper combineScopes with empty vector") {
        std::vector<WitnessScope> emptyScopes;
        REQUIRE(WitnessScopeHelper::combineScopes(emptyScopes) == 0x00);
    }
    
    SECTION("WitnessScopeHelper combineScopes with duplicate scopes") {
        std::vector<WitnessScope> scopes = {
            WitnessScope::CALLED_BY_ENTRY, 
            WitnessScope::CALLED_BY_ENTRY,  // Duplicate
            WitnessScope::CUSTOM_CONTRACTS
        };
        
        // Should still work correctly with duplicates
        REQUIRE(WitnessScopeHelper::combineScopes(scopes) == 0x11); // 0x01 | 0x10
    }
    
    SECTION("WitnessScopeHelper extractCombinedScopes") {
        std::vector<WitnessScope> extracted;
        
        // NONE scope
        extracted = WitnessScopeHelper::extractCombinedScopes(0x00);
        REQUIRE(extracted.size() == 1);
        REQUIRE(extracted[0] == WitnessScope::NONE);
        
        // Single scopes
        extracted = WitnessScopeHelper::extractCombinedScopes(0x01);
        REQUIRE(extracted.size() == 1);
        REQUIRE(extracted[0] == WitnessScope::CALLED_BY_ENTRY);
        
        extracted = WitnessScopeHelper::extractCombinedScopes(0x80);
        REQUIRE(extracted.size() == 1);
        REQUIRE(extracted[0] == WitnessScope::GLOBAL);
        
        // Combined scopes
        extracted = WitnessScopeHelper::extractCombinedScopes(0x11); // CALLED_BY_ENTRY | CUSTOM_CONTRACTS
        REQUIRE(extracted.size() == 2);
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CALLED_BY_ENTRY) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CUSTOM_CONTRACTS) != extracted.end());
        
        extracted = WitnessScopeHelper::extractCombinedScopes(0x30); // CUSTOM_CONTRACTS | CUSTOM_GROUPS
        REQUIRE(extracted.size() == 2);
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CUSTOM_CONTRACTS) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CUSTOM_GROUPS) != extracted.end());
        
        // All scopes except NONE and GLOBAL
        extracted = WitnessScopeHelper::extractCombinedScopes(0x71); // 0x01 | 0x10 | 0x20 | 0x40
        REQUIRE(extracted.size() == 4);
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CALLED_BY_ENTRY) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CUSTOM_CONTRACTS) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CUSTOM_GROUPS) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::WITNESS_RULES) != extracted.end());
    }
    
    SECTION("WitnessScopeHelper round-trip conversion") {
        // Test that toByte -> fromByte is identity
        std::vector<WitnessScope> allScopes = {
            WitnessScope::NONE,
            WitnessScope::CALLED_BY_ENTRY,
            WitnessScope::CUSTOM_CONTRACTS,
            WitnessScope::CUSTOM_GROUPS,
            WitnessScope::WITNESS_RULES,
            WitnessScope::GLOBAL
        };
        
        for (WitnessScope scope : allScopes) {
            uint8_t byte = WitnessScopeHelper::toByte(scope);
            WitnessScope converted = WitnessScopeHelper::fromByte(byte);
            REQUIRE(converted == scope);
        }
    }
    
    SECTION("WitnessScopeHelper JSON round-trip conversion") {
        // Test that toJsonString -> fromJsonString is identity
        std::vector<WitnessScope> allScopes = {
            WitnessScope::NONE,
            WitnessScope::CALLED_BY_ENTRY,
            WitnessScope::CUSTOM_CONTRACTS,
            WitnessScope::CUSTOM_GROUPS,
            WitnessScope::WITNESS_RULES,
            WitnessScope::GLOBAL
        };
        
        for (WitnessScope scope : allScopes) {
            std::string jsonStr = WitnessScopeHelper::toJsonString(scope);
            WitnessScope converted = WitnessScopeHelper::fromJsonString(jsonStr);
            REQUIRE(converted == scope);
        }
    }
    
    SECTION("WitnessScopeHelper combine/extract round-trip") {
        // Test that combineScopes -> extractCombinedScopes produces equivalent sets
        std::vector<std::vector<WitnessScope>> testCases = {
            {WitnessScope::CALLED_BY_ENTRY},
            {WitnessScope::CUSTOM_CONTRACTS},
            {WitnessScope::CALLED_BY_ENTRY, WitnessScope::CUSTOM_CONTRACTS},
            {WitnessScope::CUSTOM_CONTRACTS, WitnessScope::CUSTOM_GROUPS},
            {WitnessScope::CALLED_BY_ENTRY, WitnessScope::CUSTOM_CONTRACTS, WitnessScope::WITNESS_RULES},
            {WitnessScope::CUSTOM_CONTRACTS, WitnessScope::CUSTOM_GROUPS, WitnessScope::WITNESS_RULES}
        };
        
        for (const auto& originalScopes : testCases) {
            uint8_t combined = WitnessScopeHelper::combineScopes(originalScopes);
            auto extractedScopes = WitnessScopeHelper::extractCombinedScopes(combined);
            
            // Should have same size and contain same elements
            REQUIRE(extractedScopes.size() == originalScopes.size());
            
            for (WitnessScope scope : originalScopes) {
                REQUIRE(std::find(extractedScopes.begin(), extractedScopes.end(), scope) != extractedScopes.end());
            }
        }
    }
    
    SECTION("WitnessScope bitwise operations") {
        // Test that scopes can be combined using bitwise OR
        uint8_t combined = static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY) | 
                          static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS);
        REQUIRE(combined == 0x11);
        
        // Test that individual scope can be checked using bitwise AND
        REQUIRE((combined & static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY)) != 0);
        REQUIRE((combined & static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS)) != 0);
        REQUIRE((combined & static_cast<uint8_t>(WitnessScope::GLOBAL)) == 0);
    }
    
    SECTION("WitnessScope scope precedence and exclusivity") {
        // GLOBAL scope should be exclusive (based on Neo documentation)
        uint8_t globalOnly = static_cast<uint8_t>(WitnessScope::GLOBAL);
        REQUIRE(globalOnly == 0x80);
        
        // Combining GLOBAL with other scopes should still work technically
        uint8_t globalCombined = static_cast<uint8_t>(WitnessScope::GLOBAL) | 
                                static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY);
        REQUIRE(globalCombined == 0x81);
        
        auto extracted = WitnessScopeHelper::extractCombinedScopes(globalCombined);
        REQUIRE(extracted.size() == 2);
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::GLOBAL) != extracted.end());
        REQUIRE(std::find(extracted.begin(), extracted.end(), WitnessScope::CALLED_BY_ENTRY) != extracted.end());
    }
}