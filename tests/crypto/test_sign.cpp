#include <catch2/catch_test_macros.hpp>
#include "neocpp/crypto/sign.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/exceptions.hpp"
#include <string>
#include <vector>

using namespace neocpp;

TEST_CASE("Sign Tests", "[crypto]") {
    
    // Test data from Swift tests
    const std::string privateKeyHex = "9117f4bf9be717c9a90994326897f4243503accd06712162267e77f18b49c3a3";
    const std::string publicKeyHex = "0265bf906bf385fbf3f777832e55a87991bcfbe19b097fb7c5ca2e4025a4d5e5d6";
    const std::string testMessage = "A test message";
    const std::string expectedR = "147e5f3c929dd830d961626551dbea6b70e4b2837ed2fe9089eed2072ab3a655";
    const std::string expectedS = "523ae0fa8711eee4769f1913b180b9b3410bbb2cf770f529c85f6886f22cbaaf";
    
    SECTION("Create signature from bytes") {
        // Concatenated R and S
        std::string signatureHex = expectedR + expectedS;
        Bytes signatureBytes = Hex::decode(signatureHex);
        
        // Create signature from bytes
        ECDSASignature signature(signatureBytes);
        
        // Verify components
        REQUIRE(Hex::encode(signature.getR()) == expectedR);
        REQUIRE(Hex::encode(signature.getS()) == expectedS);
    }
    
    SECTION("Create signature from R and S") {
        Bytes rBytes = Hex::decode(expectedR);
        Bytes sBytes = Hex::decode(expectedS);
        
        ECDSASignature signature(rBytes, sBytes);
        
        REQUIRE(signature.getBytes().size() == 64);
        
        // Verify R and S
        REQUIRE(signature.getR() == rBytes);
        REQUIRE(signature.getS() == sBytes);
    }
    
    SECTION("Signature serialization") {
        // Create a signature from hex
        std::string signatureHex = expectedR + expectedS;
        ECDSASignature signature(signatureHex);
        
        // Convert to hex and back
        std::string hexOut = signature.toHex();
        REQUIRE(hexOut == signatureHex);
        
        // Create from hex again
        ECDSASignature signature2(hexOut);
        REQUIRE(signature == signature2);
    }
    
    SECTION("Canonical signatures") {
        // Create a signature
        std::string signatureHex = expectedR + expectedS;
        ECDSASignature signature(signatureHex);
        
        // Check if canonical (should be for this test data)
        bool isCanonical = signature.isCanonical();
        
        // Make canonical (should return same if already canonical)
        ECDSASignature canonical = signature.makeCanonical();
        
        if (isCanonical) {
            REQUIRE(signature == canonical);
        }
        
        // Canonical signature should always be canonical
        REQUIRE(canonical.isCanonical());
    }
    
    SECTION("Invalid signature data") {
        // Signature bytes too short
        REQUIRE_THROWS_AS(
            ECDSASignature(Bytes{1, 2, 3}),
            IllegalArgumentException
        );
        
        // R too short
        REQUIRE_THROWS_AS(
            ECDSASignature(Bytes{1}, Bytes(32, 0)),
            IllegalArgumentException
        );
        
        // S too short
        REQUIRE_THROWS_AS(
            ECDSASignature(Bytes(32, 0), Bytes{1}),
            IllegalArgumentException
        );
    }
    
    SECTION("Sign message with private key") {
        // Create private key
        Bytes privateKeyBytes = Hex::decode(privateKeyHex);
        auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);
        
        // Convert message to bytes
        Bytes messageBytes(testMessage.begin(), testMessage.end());
        
        // Sign the message
        auto signature = Sign::signMessage(messageBytes, privateKey);
        auto publicKey = privateKey->getPublicKey();
        
        // Verify signature components
        REQUIRE(signature->getR().size() == 32);
        REQUIRE(signature->getS().size() == 32);
        REQUIRE(signature->getBytes().size() == 64);
        REQUIRE(publicKey->verify(messageBytes, signature));
    }

    SECTION("Sign hash with private key") {
        Bytes privateKeyBytes = Hex::decode(privateKeyHex);
        auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);
        Bytes messageBytes(testMessage.begin(), testMessage.end());
        Bytes digest = HashUtils::sha256(messageBytes);

        auto signature = Sign::signHash(digest, privateKey);
        auto publicKey = privateKey->getPublicKey();

        REQUIRE(signature->getR().size() == 32);
        REQUIRE(signature->getS().size() == 32);
        REQUIRE(signature->getBytes().size() == 64);
        REQUIRE(publicKey->verifyHash(digest, signature));
    }
}
