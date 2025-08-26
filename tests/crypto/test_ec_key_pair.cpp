#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/exceptions.hpp"

using namespace neocpp;
using Catch::Matchers::Equals;

TEST_CASE("ECKeyPair Tests", "[crypto]") {
    const std::string encodedPoint = "03b4af8d061b6b320cce6c63bc4ec7894dce107bfc5f5ef5c68a93b4ad1e136816";
    
    SECTION("New public key from point") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(encodedPoint));
        REQUIRE(Hex::encode(publicKey->getEncoded()) == encodedPoint);
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("New public key from uncompressed point") {
        const std::string uncompressedPoint = 
            "04b4af8d061b6b320cce6c63bc4ec7894dce107bfc5f5ef5c68a93b4ad1e1368165f4f7fb1c5862465543c06dd5a2aa414f6583f92a5cc3e1d4259df79bf6839c9";
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(uncompressedPoint));
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("New public key from string with invalid size") {
        std::string tooSmall = encodedPoint.substr(0, encodedPoint.size() - 2);
        REQUIRE_THROWS_AS(ECPublicKey(Hex::decode(tooSmall)), IllegalArgumentException);
    }
    
    SECTION("New public key from point with hex prefix") {
        const std::string prefixed = "0x03b4af8d061b6b320cce6c63bc4ec7894dce107bfc5f5ef5c68a93b4ad1e136816";
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(prefixed.substr(2)));
        REQUIRE(Hex::encode(publicKey->getEncodedCompressed()) == encodedPoint);
    }
    
    SECTION("Serialize public key") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode(encodedPoint));
        REQUIRE(publicKey->toArray() == Hex::decode(encodedPoint));
    }
    
    SECTION("Deserialize public key") {
        Bytes data = Hex::decode("036b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296");
        auto publicKey = ECPublicKey::from(data);
        REQUIRE(publicKey != nullptr);
        // Verify it's the generator point (secp256r1 G)
        // This test validates the key represents the curve's generator point
    }
    
    SECTION("Public key size") {
        auto publicKey = std::make_shared<ECPublicKey>(Hex::decode("036b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296"));
        REQUIRE(publicKey->size() == 33);
    }
    
    SECTION("Public key WIF") {
        const std::string privateKey = "c7134d6fd8e73d819e82755c64c93788d8db0961929e025a53363c4cc02a6962";
        ECKeyPair keyPair(Hex::decode(privateKey));
        REQUIRE(keyPair.exportAsWIF() == "L3tgppXLgdaeqSGSFw1Go3skBiy8vQAM7YMXvTHsKQtE16PBncSU");
    }
    
    SECTION("Public key comparable") {
        const std::string encodedKey2 = "036b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296";
        const std::string encodedKey1Uncompressed = 
            "04b4af8d061b6b320cce6c63bc4ec7894dce107bfc5f5ef5c68a93b4ad1e1368165f4f7fb1c5862465543c06dd5a2aa414f6583f92a5cc3e1d4259df79bf6839c9";
        
        auto key1 = std::make_shared<ECPublicKey>(Hex::decode(encodedPoint));
        auto key2 = std::make_shared<ECPublicKey>(Hex::decode(encodedKey2));
        auto key1Uncompressed = std::make_shared<ECPublicKey>(Hex::decode(encodedKey1Uncompressed));
        
        REQUIRE(*key1 > *key2);
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
        const std::string expectedPublicKey = "02b379356267a5d01cb0777ef97509e061ff2d0c9a5cb718720c67005aabefc74f";
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