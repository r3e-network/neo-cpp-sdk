#include <catch2/catch_test_macros.hpp>
#include "neocpp/crypto/wif.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/utils/base58.hpp"
#include "neocpp/exceptions.hpp"

using namespace neocpp;

TEST_CASE("WIF Tests", "[crypto]") {
    
    SECTION("Encode private key to WIF") {
        const std::string privateKeyHex = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        Bytes privateKey = Hex::decode(privateKeyHex);
        
        std::string wif = WIF::encode(privateKey);
        const std::string expected = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        REQUIRE(wif == expected);
    }

    SECTION("Encode wrongly sized private key throws exception") {
        // Test with private key that's too short
        Bytes tooShortPrivateKey(31, 0x00);
        REQUIRE_THROWS_AS(WIF::encode(tooShortPrivateKey), IllegalArgumentException);
        
        // Test with private key that's too long
        Bytes tooLongPrivateKey(33, 0x00);
        REQUIRE_THROWS_AS(WIF::encode(tooLongPrivateKey), IllegalArgumentException);
    }
    
    SECTION("Decode WIF to private key") {
        const std::string wif = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        Bytes privateKey = WIF::decode(wif);
        
        const std::string expected = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        REQUIRE(Hex::encode(privateKey) == expected);
    }
    
    SECTION("Validate valid WIF") {
        const std::string validWif = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        REQUIRE(WIF::isValid(validWif));
    }
    
    SECTION("Validate invalid WIF") {
        REQUIRE_FALSE(WIF::isValid("InvalidWIF"));
        REQUIRE_FALSE(WIF::isValid(""));
        REQUIRE_FALSE(WIF::isValid("KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYn")); // Too short
        REQUIRE_FALSE(WIF::isValid("KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnZ")); // Wrong checksum
    }

    SECTION("Invalid WIF with wrong first byte") {
        // Create a valid WIF and modify the first byte
        std::string validWif = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        Bytes decoded = Base58::decode(validWif);
        REQUIRE(!decoded.empty());
        decoded[0] = 0x81; // Change version byte from 0x80 to 0x81
        std::string invalidWif = Base58::encode(decoded);
        REQUIRE_FALSE(WIF::isValid(invalidWif));
        REQUIRE_THROWS_AS(WIF::decode(invalidWif), CryptoException);
    }

    SECTION("Invalid WIF with wrong compression flag") {
        // Create a valid WIF and modify the compression flag
        std::string validWif = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        Bytes decoded = Base58::decode(validWif);
        REQUIRE(!decoded.empty());
        decoded[33] = 0x00; // Change compression flag from 0x01 to 0x00
        std::string invalidWif = Base58::encode(decoded);
        REQUIRE_FALSE(WIF::isValid(invalidWif));
        REQUIRE_THROWS_AS(WIF::decode(invalidWif), CryptoException);
    }
    
    SECTION("Decode invalid WIF throws exception") {
        REQUIRE_THROWS_AS(WIF::decode("InvalidWIF"), CryptoException);
        REQUIRE_THROWS_AS(WIF::decode(""), CryptoException);
        REQUIRE_THROWS_AS(WIF::decode("KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYn"), CryptoException); // Too short
        REQUIRE_THROWS_AS(WIF::decode("KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnZ"), CryptoException); // Wrong checksum
    }
    
    SECTION("Round trip encoding and decoding") {
        // Generate random private key
        ECKeyPair keyPair = ECKeyPair::generate();
        Bytes originalPrivateKey = keyPair.getPrivateKey()->getBytes();
        
        // Encode to WIF
        std::string wif = WIF::encode(originalPrivateKey);
        REQUIRE(!wif.empty());
        REQUIRE(WIF::isValid(wif));
        
        // Decode back
        Bytes decodedPrivateKey = WIF::decode(wif);
        REQUIRE(decodedPrivateKey == originalPrivateKey);
    }
    
    SECTION("Multiple known WIF test vectors") {
        struct TestVector {
            std::string privateKeyHex;
            std::string wif;
        };
        
        std::vector<TestVector> testVectors = {
            {"1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb", 
             "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr"},
            {"c983f3c6effa11a996b8def32f7ddca2a94c0c983b8a6025bb7a50f45e1cf647",
             "L3yRvfEowWQx7VvH5n45T54rkmbwKjSP728m913EdKQVUNMebQNv"},
            {"9117f0fe33bde89d70776e5e601704026db68e09ca456707093eb94e28c0b450",
             "L25kfnfjZiifZo8MZpwLsD4k73v3x4AjUP91tGVq3DKj33BZLBjs"}
        };
        
        for (const auto& tv : testVectors) {
            // Test encoding
            Bytes privateKey = Hex::decode(tv.privateKeyHex);
            REQUIRE(WIF::encode(privateKey) == tv.wif);
            
            // Test decoding
            Bytes decodedKey = WIF::decode(tv.wif);
            REQUIRE(Hex::encode(decodedKey) == tv.privateKeyHex);
            
            // Test validation
            REQUIRE(WIF::isValid(tv.wif));
        }
    }
    
    SECTION("WIF with uncompressed key format") {
        // Note: Neo uses compressed format by default
        // This test ensures we handle the compressed format correctly
        const std::string wif = "L3yRvfEowWQx7VvH5n45T54rkmbwKjSP728m913EdKQVUNMebQNv";
        REQUIRE(WIF::isValid(wif));
        
        Bytes privateKey = WIF::decode(wif);
        REQUIRE(privateKey.size() == 32);
    }
}