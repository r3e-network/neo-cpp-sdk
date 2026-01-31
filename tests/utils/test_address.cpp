#include <catch2/catch_test_macros.hpp>
#include "neocpp/utils/address.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/script/script_builder.hpp"
#include <string>
#include <vector>

using namespace neocpp;

TEST_CASE("AddressUtils Tests", "[utils]") {
    
    SECTION("Create address from script hash") {
        // Create a hash160 from bytes
        Bytes scriptHashBytes = Hex::decode("23ba2703c53263e8d6e522dc32203339dcd8eee9");
        
        std::string address = AddressUtils::scriptHashToAddress(scriptHashBytes);
        
        REQUIRE(!address.empty());
        REQUIRE(address[0] == 'N'); // Neo N3 addresses start with 'N'
        REQUIRE(address.length() == 34); // Standard Neo address length
    }
    
    SECTION("Convert address to script hash") {
        std::string address = "NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj";
        
        Bytes scriptHash = AddressUtils::addressToScriptHash(address);
        
        REQUIRE(!scriptHash.empty());
        REQUIRE(scriptHash.size() == 20);
        
        // Convert back to address
        std::string recoveredAddress = AddressUtils::scriptHashToAddress(scriptHash);
        REQUIRE(recoveredAddress == address);
    }
    
    SECTION("Address validation") {
        // Valid addresses
        REQUIRE(AddressUtils::isValidAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj") == true);
        REQUIRE(AddressUtils::isValidAddress("NfuwpaQ1A2xaeVbxWe8FRtaRgaMa8yF3YM") == true);
        
        // Invalid addresses
        REQUIRE(AddressUtils::isValidAddress("") == false);
        REQUIRE(AddressUtils::isValidAddress("InvalidAddress") == false);
        REQUIRE(AddressUtils::isValidAddress("MZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj") == false); // Wrong prefix
        REQUIRE(AddressUtils::isValidAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYA") == false); // Too short
        REQUIRE(AddressUtils::isValidAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAajXX") == false); // Too long
    }
    
    SECTION("Create address from public key") {
        ECKeyPair keyPair = ECKeyPair::generate();
        
        // Create verification script from public key
        Bytes verificationScript = ScriptBuilder::buildVerificationScript(keyPair.getPublicKey());
        
        // Hash the verification script
        Bytes scriptHash = HashUtils::sha256ThenRipemd160(verificationScript);
        
        // Create address
        std::string address = AddressUtils::scriptHashToAddress(scriptHash);
        
        REQUIRE(!address.empty());
        REQUIRE(address[0] == 'N');
        REQUIRE(address.length() == 34);
        REQUIRE(AddressUtils::isValidAddress(address) == true);
    }
    
    SECTION("Create address from verification script") {
        ECKeyPair keyPair = ECKeyPair::generate();
        Bytes verificationScript = ScriptBuilder::buildVerificationScript(keyPair.getPublicKey());
        
        // Hash the verification script
        Bytes scriptHash = HashUtils::sha256ThenRipemd160(verificationScript);
        std::string address = AddressUtils::scriptHashToAddress(scriptHash);
        
        REQUIRE(!address.empty());
        REQUIRE(address[0] == 'N');
        REQUIRE(address.length() == 34);
        REQUIRE(AddressUtils::isValidAddress(address) == true);
    }
    
    SECTION("Round-trip conversion") {
        // Test with known script hash
        Bytes originalScriptHash = Hex::decode("abcdef1234567890abcdef1234567890abcdef12");
        
        // Convert to address
        std::string address = AddressUtils::scriptHashToAddress(originalScriptHash);
        
        // Convert back to script hash
        Bytes recoveredScriptHash = AddressUtils::addressToScriptHash(address);
        
        REQUIRE(recoveredScriptHash == originalScriptHash);
        REQUIRE(AddressUtils::isValidAddress(address) == true);
    }
    
    SECTION("Known address test vectors") {
        // Test with known Neo addresses
        std::vector<std::string> validAddresses = {
            "NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj",
            "NfuwpaQ1A2xaeVbxWe8FRtaRgaMa8yF3YM"
        };
        
        for (const auto& addr : validAddresses) {
            if (AddressUtils::isValidAddress(addr)) {
                Bytes scriptHash = AddressUtils::addressToScriptHash(addr);
                std::string recovered = AddressUtils::scriptHashToAddress(scriptHash);
                REQUIRE(recovered == addr);
            }
        }
    }
    
    SECTION("Multi-signature address") {
        // Create multiple public keys
        std::vector<SharedPtr<ECPublicKey>> publicKeys;
        for (int i = 0; i < 3; i++) {
            ECKeyPair keyPair = ECKeyPair::generate();
            publicKeys.push_back(keyPair.getPublicKey());
        }
        
        // Create 2-of-3 multi-sig verification script
        Bytes verificationScript = ScriptBuilder::buildVerificationScript(publicKeys, 2);
        
        // Hash the verification script
        Bytes scriptHash = HashUtils::sha256ThenRipemd160(verificationScript);
        std::string multiSigAddress = AddressUtils::scriptHashToAddress(scriptHash);
        
        REQUIRE(!multiSigAddress.empty());
        REQUIRE(multiSigAddress[0] == 'N');
        REQUIRE(multiSigAddress.length() == 34);
        REQUIRE(AddressUtils::isValidAddress(multiSigAddress) == true);
    }
    
    SECTION("Error handling for invalid input") {
        // Empty address
        REQUIRE(AddressUtils::isValidAddress("") == false);
        
        // Invalid characters
        REQUIRE(AddressUtils::isValidAddress("N$#@!2WqTbu5oCgyfss9kUJgBXJqhuYAaj") == false);
        
        // Wrong length
        REQUIRE(AddressUtils::isValidAddress("NZN") == false);
        REQUIRE(AddressUtils::isValidAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAajExtraChars") == false);
    }
    
    SECTION("Address version byte") {
        uint8_t version = AddressUtils::getAddressVersion();
        
        // Neo N3 mainnet version
        REQUIRE(version == 0x35); // Neo N3 uses 0x35 (53 decimal)
    }
}