#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/exceptions.hpp"

using namespace neocpp;
using Catch::Matchers::Equals;

TEST_CASE("ECKeyPair Tests", "[crypto]") {
    // Use secp256r1 (P-256) test vectors - Neo N3 uses secp256r1, NOT secp256k1
    const std::string encodedPoint = "031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a";
    
    SECTION("New public key from point") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(encodedPoint));
        REQUIRE(Hex::encode(publicKey->getEncoded()) == encodedPoint);
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("New public key from uncompressed point") {
        // Uncompressed secp256r1 public key: 0x04 || X || Y
        const std::string uncompressedPoint = 
            "041a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a6e3ae669b7a7126ebd9495ac304e44b89b1f3a3a85922c2b9b5aafa8acec98b1";
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(uncompressedPoint));
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("New public key from string with invalid size") {
        std::string tooSmall = encodedPoint.substr(0, encodedPoint.size() - 2);
        REQUIRE_THROWS_AS(ECPublicKey(Hex::decode(tooSmall)), IllegalArgumentException);
    }
    
    SECTION("New public key from point with hex prefix") {
        const std::string prefixed = "0x031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a";
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(prefixed.substr(2)));
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("Serialize public key") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(encodedPoint));
        REQUIRE(publicKey->toArray() == Hex::decode(encodedPoint));
    }
    
    SECTION("Deserialize public key") {
        // Valid secp256r1 public key
        Bytes data = Hex::decode("031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a");
        auto publicKey = ECPublicKey::from(data);
        REQUIRE(publicKey != nullptr);
    }
    
    SECTION("Public key size") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode("031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a"));
        REQUIRE(publicKey->size() == 33);
    }
    
    SECTION("Public key WIF") {
        // secp256r1 private key and expected WIF
        const std::string privateKey = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        ECKeyPair keyPair(Hex::decode(privateKey));
        REQUIRE(keyPair.exportAsWIF() == "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr");
    }
    
    SECTION("Public key comparable") {
        // Use secp256r1 (P-256) test vectors - Neo N3 standard
        // These keys are valid on the secp256r1 curve
        const std::string encodedKey1 = "031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a";
        const std::string encodedKey1Uncompressed = 
            "041a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a6e3ae669b7a7126ebd9495ac304e44b89b1f3a3a85922c2b9b5aafa8acec98b1";
        const std::string encodedKey2 = "03ea182d585287444c85cecb8239508d0becb3afe39e2a8d4fb7559c68e0ee3ab4";
        
        auto key1 = std::make_shared<ECPublicKey>(Hex::decode(encodedKey1));
        auto key2 = std::make_shared<ECPublicKey>(Hex::decode(encodedKey2));
        auto key1Uncompressed = std::make_shared<ECPublicKey>(Hex::decode(encodedKey1Uncompressed));
        
        // key1 < key2 (based on byte comparison - 0x1a < 0xea at position 1)
        REQUIRE(*key1 < *key2);
        // Compressed and uncompressed forms represent the same key
        REQUIRE(*key1 == *key1Uncompressed);
        REQUIRE_FALSE(*key1 < *key1Uncompressed);
        REQUIRE_FALSE(*key1 > *key1Uncompressed);
    }
    
    SECTION("Generate new key pair") {
        auto keyPair = ECKeyPair::generate();
        REQUIRE(keyPair.getPrivateKey() != nullptr);
        REQUIRE(keyPair.getPublicKey() != nullptr);
        REQUIRE(keyPair.getPrivateKey()->getBytes().size() == 32);
        REQUIRE(keyPair.getPublicKey()->getEncoded().size() == 33);
    }
    
    SECTION("Create key pair from private key") {
        const std::string privateKeyHex = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        auto privateKeyBytes = Hex::decode(privateKeyHex);
        ECKeyPair keyPair(privateKeyBytes);
        
        REQUIRE(Hex::encode(keyPair.getPrivateKey()->getBytes()) == privateKeyHex);
        // Note: This is the secp256r1 (P-256) public key, NOT secp256k1
        // Neo N3 uses secp256r1 elliptic curve
        const std::string expectedPublicKey = "031a6c6fbbdf02ca351745fa86b9ba5a9452d785ac4f7fc2b7548ca2a46c4fcf4a";
        REQUIRE(Hex::encode(keyPair.getPublicKey()->getEncoded()) == expectedPublicKey);
    }
    
    SECTION("Create key pair from WIF") {
        const std::string wif = "L3yRvfEowWQx7VvH5n45T54rkmbwKjSP728m913EdKQVUNMebQNv";
        ECKeyPair keyPair = ECKeyPair::fromWIF(wif);
        
        const std::string expectedPrivateKey = "c983f3c6effa11a996b8def32f7ddca2a94c0c983b8a6025bb7a50f45e1cf647";
        REQUIRE(Hex::encode(keyPair.getPrivateKey()->getBytes()) == expectedPrivateKey);
    }
    
    SECTION("Export key pair as WIF") {
        const std::string privateKeyHex = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        ECKeyPair keyPair(Hex::decode(privateKeyHex));
        
        const std::string expectedWif = "KxDgvEKzgSBPPfuVfw67oPQBSjidEiqTHURKSDL1R7yGaGYAeYnr";
        REQUIRE(keyPair.exportAsWIF() == expectedWif);
    }
    
    SECTION("Sign and verify message") {
        const std::string privateKeyHex = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        ECKeyPair keyPair(Hex::decode(privateKeyHex));
        
        Bytes message = {0x01, 0x02, 0x03, 0x04, 0x05};
        auto signature = keyPair.sign(message);
        
        REQUIRE(signature != nullptr);
        REQUIRE(keyPair.getPublicKey()->verify(message, signature));
        
        // Verify with wrong message should fail
        Bytes wrongMessage = {0x01, 0x02, 0x03, 0x04, 0x06};
        REQUIRE_FALSE(keyPair.getPublicKey()->verify(wrongMessage, signature));
    }
    
    SECTION("Key pair equality") {
        const std::string privateKeyHex = "1dd37fba80fec4e6a6f13fd708d8dcb3b29def768017052f6c930fa1c5d90bbb";
        ECKeyPair keyPair1(Hex::decode(privateKeyHex));
        ECKeyPair keyPair2(Hex::decode(privateKeyHex));
        
        // Compare private keys
        REQUIRE(keyPair1.getPrivateKey()->getBytes() == keyPair2.getPrivateKey()->getBytes());
        
        ECKeyPair keyPair3 = ECKeyPair::generate();
        REQUIRE_FALSE(keyPair1.getPrivateKey()->getBytes() == keyPair3.getPrivateKey()->getBytes());
    }
}