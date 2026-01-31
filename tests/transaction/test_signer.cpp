#include <catch2/catch_test_macros.hpp>
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness_rule.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/neo_constants.hpp"
#include "neocpp/utils/hex.hpp"

using namespace neocpp;

TEST_CASE("Signer Tests", "[transaction]") {
    
    SECTION("Constructor with account and default scope") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account);
        
        REQUIRE(signer.getAccount() == account);
        REQUIRE(signer.getScopes() == WitnessScope::CALLED_BY_ENTRY);
        REQUIRE(signer.getAllowedContracts().empty());
        REQUIRE(signer.getAllowedGroups().empty());
        REQUIRE(signer.getRules().empty());
    }
    
    SECTION("Constructor with account and custom scope") {
        Hash160 account("1234567890123456789012345678901234567890");
        WitnessScope scope = WitnessScope::GLOBAL;
        Signer signer(account, scope);
        
        REQUIRE(signer.getAccount() == account);
        REQUIRE(signer.getScopes() == scope);
    }
    
    SECTION("Set and get scopes") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account);
        
        signer.setScopes(WitnessScope::CUSTOM_CONTRACTS);
        REQUIRE(signer.getScopes() == WitnessScope::CUSTOM_CONTRACTS);
        
        signer.setScopes(WitnessScope::GLOBAL);
        REQUIRE(signer.getScopes() == WitnessScope::GLOBAL);
    }
    
    SECTION("Add allowed contracts") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_CONTRACTS);
        
        Hash160 contract1("abcdef0123456789012345678901234567890123");
        Hash160 contract2("fedcba9876543210987654321098765432109876");
        
        signer.addAllowedContract(contract1);
        signer.addAllowedContract(contract2);
        
        const auto& contracts = signer.getAllowedContracts();
        REQUIRE(contracts.size() == 2);
        REQUIRE(contracts[0] == contract1);
        REQUIRE(contracts[1] == contract2);
    }
    
    SECTION("Add allowed groups") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_GROUPS);
        
        Bytes group1(33, 0x02); // Compressed public key starting with 0x02
        group1[32] = 0x01;
        
        Bytes group2(33, 0x03); // Compressed public key starting with 0x03
        group2[32] = 0x02;
        
        signer.addAllowedGroup(group1);
        signer.addAllowedGroup(group2);
        
        const auto& groups = signer.getAllowedGroups();
        REQUIRE(groups.size() == 2);
        REQUIRE(groups[0] == group1);
        REQUIRE(groups[1] == group2);
    }
    
    SECTION("Check global scope") {
        Hash160 account("1234567890123456789012345678901234567890");
        
        Signer globalSigner(account, WitnessScope::GLOBAL);
        REQUIRE(globalSigner.isGlobal());
        
        Signer nonGlobalSigner(account, WitnessScope::CALLED_BY_ENTRY);
        REQUIRE_FALSE(nonGlobalSigner.isGlobal());
    }
    
    SECTION("Check has scope") {
        Hash160 account("1234567890123456789012345678901234567890");
        
        Signer signer(account, WitnessScope::CALLED_BY_ENTRY);
        REQUIRE(signer.hasScope(WitnessScope::CALLED_BY_ENTRY));
        REQUIRE_FALSE(signer.hasScope(WitnessScope::GLOBAL));
        REQUIRE_FALSE(signer.hasScope(WitnessScope::CUSTOM_CONTRACTS));
        
        // Test combined scopes using bitwise OR
        uint8_t combinedScope = static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY) |
                               static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS);
        WitnessScope combined = static_cast<WitnessScope>(combinedScope);
        
        Signer combinedSigner(account, combined);
        REQUIRE(combinedSigner.hasScope(WitnessScope::CALLED_BY_ENTRY));
        REQUIRE(combinedSigner.hasScope(WitnessScope::CUSTOM_CONTRACTS));
        REQUIRE_FALSE(combinedSigner.hasScope(WitnessScope::GLOBAL));
    }
    
    SECTION("Get size calculation") {
        Hash160 account("1234567890123456789012345678901234567890");
        
        // Basic signer with CALLED_BY_ENTRY scope
        Signer basicSigner(account, WitnessScope::CALLED_BY_ENTRY);
        size_t basicSize = basicSigner.getSize();
        REQUIRE(basicSize == 20 + 1); // Hash160 + scope byte
        
        // Signer with custom contracts
        Signer contractSigner(account, WitnessScope::CUSTOM_CONTRACTS);
        Hash160 contract("abcdef0123456789012345678901234567890123");
        contractSigner.addAllowedContract(contract);
        
        size_t contractSize = contractSigner.getSize();
        REQUIRE(contractSize == 20 + 1 + 1 + 20); // Hash160 + scope + var_int + contract Hash160
        
        // Signer with custom groups
        Signer groupSigner(account, WitnessScope::CUSTOM_GROUPS);
        Bytes group(33, 0x02);
        groupSigner.addAllowedGroup(group);
        
        size_t groupSize = groupSigner.getSize();
        REQUIRE(groupSize == 20 + 1 + 1 + 33); // Hash160 + scope + var_int + public key
    }
    
    SECTION("Serialization and deserialization - basic signer") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer original(account, WitnessScope::CALLED_BY_ENTRY);
        
        BinaryWriter writer;
        original.serialize(writer);
        Bytes serialized = writer.toArray();
        
        BinaryReader reader(serialized);
        auto deserialized = Signer::deserialize(reader);
        
        REQUIRE(deserialized->getAccount() == original.getAccount());
        REQUIRE(deserialized->getScopes() == original.getScopes());
        REQUIRE(*deserialized == original);
    }
    
    SECTION("Serialization and deserialization - with contracts") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer original(account, WitnessScope::CUSTOM_CONTRACTS);
        
        Hash160 contract1("abcdef0123456789012345678901234567890123");
        Hash160 contract2("fedcba9876543210987654321098765432109876");
        original.addAllowedContract(contract1);
        original.addAllowedContract(contract2);
        
        BinaryWriter writer;
        original.serialize(writer);
        Bytes serialized = writer.toArray();
        
        BinaryReader reader(serialized);
        auto deserialized = Signer::deserialize(reader);
        
        REQUIRE(deserialized->getAccount() == original.getAccount());
        REQUIRE(deserialized->getScopes() == original.getScopes());
        REQUIRE(deserialized->getAllowedContracts().size() == 2);
        REQUIRE(deserialized->getAllowedContracts()[0] == contract1);
        REQUIRE(deserialized->getAllowedContracts()[1] == contract2);
        REQUIRE(*deserialized == original);
    }
    
    SECTION("Serialization and deserialization - with groups") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer original(account, WitnessScope::CUSTOM_GROUPS);
        
        Bytes group1(33, 0x02);
        group1[32] = 0x01;
        Bytes group2(33, 0x03);  
        group2[32] = 0x02;
        
        original.addAllowedGroup(group1);
        original.addAllowedGroup(group2);
        
        BinaryWriter writer;
        original.serialize(writer);
        Bytes serialized = writer.toArray();
        
        BinaryReader reader(serialized);
        auto deserialized = Signer::deserialize(reader);
        
        REQUIRE(deserialized->getAccount() == original.getAccount());
        REQUIRE(deserialized->getScopes() == original.getScopes());
        REQUIRE(deserialized->getAllowedGroups().size() == 2);
        REQUIRE(deserialized->getAllowedGroups()[0] == group1);
        REQUIRE(deserialized->getAllowedGroups()[1] == group2);
        REQUIRE(*deserialized == original);
    }
    
    SECTION("Equality operators") {
        Hash160 account1("1234567890123456789012345678901234567890");
        Hash160 account2("abcdef0123456789012345678901234567890123");
        
        Signer signer1(account1, WitnessScope::CALLED_BY_ENTRY);
        Signer signer2(account1, WitnessScope::CALLED_BY_ENTRY);
        Signer signer3(account2, WitnessScope::CALLED_BY_ENTRY);
        Signer signer4(account1, WitnessScope::GLOBAL);
        
        REQUIRE(signer1 == signer2);
        REQUIRE_FALSE(signer1 != signer2);
        
        REQUIRE_FALSE(signer1 == signer3); // Different accounts
        REQUIRE(signer1 != signer3);
        
        REQUIRE_FALSE(signer1 == signer4); // Different scopes
        REQUIRE(signer1 != signer4);
    }
    
    SECTION("Maximum allowed contracts limit") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_CONTRACTS);
        
        // Add maximum allowed contracts
        for (int i = 0; i < NeoConstants::MAX_SIGNER_SUBITEMS; i++) {
            std::string hashStr = "123456789012345678901234567890123456789" + std::to_string(i);
            Hash160 contract(hashStr.substr(0, 40)); // Ensure exactly 40 hex chars
            signer.addAllowedContract(contract);
        }
        
        REQUIRE(signer.getAllowedContracts().size() == NeoConstants::MAX_SIGNER_SUBITEMS);
        
        // Adding one more should throw exception
        Hash160 extraContract("fedcba9876543210987654321098765432109876");
        REQUIRE_THROWS_AS(signer.addAllowedContract(extraContract), TransactionException);
    }
    
    SECTION("Maximum allowed groups limit") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_GROUPS);
        
        // Add maximum allowed groups
        for (int i = 0; i < NeoConstants::MAX_SIGNER_SUBITEMS; i++) {
            Bytes group(33, static_cast<uint8_t>(0x02 + i));
            signer.addAllowedGroup(group);
        }
        
        REQUIRE(signer.getAllowedGroups().size() == NeoConstants::MAX_SIGNER_SUBITEMS);
        
        // Adding one more should throw exception
        Bytes extraGroup(33, 0xFF);
        REQUIRE_THROWS_AS(signer.addAllowedGroup(extraGroup), TransactionException);
    }
    
    SECTION("JSON serialization") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_CONTRACTS);
        
        Hash160 contract("abcdef0123456789012345678901234567890123");
        signer.addAllowedContract(contract);
        
        nlohmann::json json = signer.toJson();
        
        REQUIRE(json["account"] == account.toString());
        REQUIRE(json["scopes"].is_array());
        REQUIRE(json["scopes"].size() == 1);
        REQUIRE(json["scopes"][0] == "CustomContracts");
        REQUIRE(json["allowedcontracts"].size() == 1);
        REQUIRE(json["allowedcontracts"][0] == contract.toString());
    }
    
    SECTION("JSON serialization with groups") {
        Hash160 account("1234567890123456789012345678901234567890");
        Signer signer(account, WitnessScope::CUSTOM_GROUPS);
        
        Bytes group(33, 0x02);
        group[32] = 0x01;
        signer.addAllowedGroup(group);
        
        nlohmann::json json = signer.toJson();
        
        REQUIRE(json["account"] == account.toString());
        REQUIRE(json["scopes"].is_array());
        REQUIRE(json["scopes"].size() == 1);
        REQUIRE(json["scopes"][0] == "CustomGroups");
        REQUIRE(json["allowedgroups"].size() == 1);
        REQUIRE(json["allowedgroups"][0] == Hex::encode(group));
    }
    
    SECTION("Combined scopes serialization") {
        Hash160 account("1234567890123456789012345678901234567890");
        
        // Create a signer with combined scopes (bitwise OR)
        uint8_t combinedScope = static_cast<uint8_t>(WitnessScope::CALLED_BY_ENTRY) |
                               static_cast<uint8_t>(WitnessScope::CUSTOM_CONTRACTS);
        WitnessScope combined = static_cast<WitnessScope>(combinedScope);
        
        Signer signer(account, combined);
        Hash160 contract("abcdef0123456789012345678901234567890123");
        signer.addAllowedContract(contract);
        
        BinaryWriter writer;
        signer.serialize(writer);
        Bytes serialized = writer.toArray();
        
        BinaryReader reader(serialized);
        auto deserialized = Signer::deserialize(reader);
        
        REQUIRE(deserialized->getScopes() == combined);
        REQUIRE(deserialized->hasScope(WitnessScope::CALLED_BY_ENTRY));
        REQUIRE(deserialized->hasScope(WitnessScope::CUSTOM_CONTRACTS));
        REQUIRE(deserialized->getAllowedContracts().size() == 1);
    }
}
