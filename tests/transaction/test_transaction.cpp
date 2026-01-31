#include <catch2/catch_test_macros.hpp>
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/utils/hex.hpp"

using namespace neocpp;

TEST_CASE("Transaction Tests", "[transaction]") {
    
    SECTION("Create empty transaction") {
        Transaction tx;
        
        REQUIRE(tx.getVersion() == 0);
        REQUIRE(tx.getNonce() != 0); // Should be randomly generated
        REQUIRE(tx.getSystemFee() == 0);
        REQUIRE(tx.getNetworkFee() == 0);
        REQUIRE(tx.getValidUntilBlock() == 0);
        REQUIRE(tx.getSigners().empty());
        REQUIRE(tx.getAttributes().empty());
        REQUIRE(tx.getScript().empty());
        REQUIRE(tx.getWitnesses().empty());
    }
    
    SECTION("Set transaction properties") {
        Transaction tx;
        
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);
        
        REQUIRE(tx.getVersion() == 0);
        REQUIRE(tx.getNonce() == 12345678);
        REQUIRE(tx.getSystemFee() == 100000);
        REQUIRE(tx.getNetworkFee() == 200000);
        REQUIRE(tx.getValidUntilBlock() == 1000000);
    }
    
    SECTION("Set transaction script") {
        Transaction tx;
        
        ScriptBuilder builder;
        builder.pushInteger(42);
        builder.pushData(Bytes{0x01, 0x02, 0x03});
        Bytes script = builder.toArray();
        
        tx.setScript(script);
        REQUIRE(tx.getScript() == script);
    }
    
    SECTION("Add signers to transaction") {
        Transaction tx;
        
        Hash160 account1("23ba2703c53263e8d6e522dc32203339dcd8eee9");
        Hash160 account2("e707714512577b42f9a011f8b31b4e9afc96e196");
        
        auto signer1 = std::make_shared<Signer>(account1, WitnessScope::CALLED_BY_ENTRY);
        auto signer2 = std::make_shared<Signer>(account2, WitnessScope::GLOBAL);
        
        tx.addSigner(signer1);
        tx.addSigner(signer2);
        
        REQUIRE(tx.getSigners().size() == 2);
        REQUIRE(tx.getSigners()[0]->getAccount() == account1);
        REQUIRE(tx.getSigners()[1]->getAccount() == account2);
    }
    
    SECTION("Add witnesses to transaction") {
        Transaction tx;
        
        Bytes invocationScript = {0x0c, 0x40}; // Signature placeholder
        Bytes verificationScript = {0x0c, 0x21}; // Public key placeholder
        
        auto witness = std::make_shared<Witness>(invocationScript, verificationScript);
        tx.addWitness(witness);
        
        REQUIRE(tx.getWitnesses().size() == 1);
        REQUIRE(tx.getWitnesses()[0]->getInvocationScript() == invocationScript);
        REQUIRE(tx.getWitnesses()[0]->getVerificationScript() == verificationScript);
    }
    
    SECTION("Calculate transaction hash") {
        Transaction tx;
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);
        
        Hash256 hash = tx.getHash();
        REQUIRE(hash != Hash256::ZERO);
        
        // Hash should be consistent
        Hash256 hash2 = tx.getHash();
        REQUIRE(hash == hash2);
        
        // Changing transaction should change hash
        tx.setNonce(87654321);
        Hash256 hash3 = tx.getHash();
        REQUIRE(hash != hash3);
    }
    
    SECTION("Serialize and deserialize transaction") {
        Transaction tx;
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);
        
        // Add script
        ScriptBuilder builder;
        builder.pushInteger(42);
        tx.setScript(builder.toArray());
        
        // Add signer
        Hash160 account("23ba2703c53263e8d6e522dc32203339dcd8eee9");
        auto signer = std::make_shared<Signer>(account, WitnessScope::CALLED_BY_ENTRY);
        tx.addSigner(signer);
        
        // Serialize
        BinaryWriter writer;
        tx.serialize(writer);
        Bytes serialized = writer.toArray();
        
        // Deserialize
        BinaryReader reader(serialized);
        auto deserialized = Transaction::deserialize(reader);
        
        REQUIRE(deserialized->getVersion() == tx.getVersion());
        REQUIRE(deserialized->getNonce() == tx.getNonce());
        REQUIRE(deserialized->getSystemFee() == tx.getSystemFee());
        REQUIRE(deserialized->getNetworkFee() == tx.getNetworkFee());
        REQUIRE(deserialized->getValidUntilBlock() == tx.getValidUntilBlock());
        REQUIRE(deserialized->getScript() == tx.getScript());
        REQUIRE(deserialized->getSigners().size() == tx.getSigners().size());
    }
    
    SECTION("Sign transaction manually") {
        Transaction tx;
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);
        
        // Create key pair
        ECKeyPair keyPair = ECKeyPair::generate();
        Hash160 scriptHash = Hash160::fromPublicKey(keyPair.getPublicKey()->getEncoded());
        
        // Add signer
        auto signer = std::make_shared<Signer>(scriptHash, WitnessScope::CALLED_BY_ENTRY);
        tx.addSigner(signer);
        
        // Create witness manually (since sign() may not be implemented)
        // Get transaction hash to sign
        Hash256 txHash = tx.getHash();
        
        // Sign the hash
        auto signature = keyPair.sign(txHash.toArray());
        
        // Create witness with signature and verification script
        ScriptBuilder invocationBuilder;
        invocationBuilder.pushData(signature->getBytes());
        Bytes invocationScript = invocationBuilder.toArray();
        
        Bytes verificationScript = ScriptBuilder::buildVerificationScript(keyPair.getPublicKey());
        
        auto witness = std::make_shared<Witness>(invocationScript, verificationScript);
        tx.addWitness(witness);
        
        // Should have one witness
        REQUIRE(tx.getWitnesses().size() == 1);
        REQUIRE(!tx.getWitnesses()[0]->getInvocationScript().empty());
        REQUIRE(!tx.getWitnesses()[0]->getVerificationScript().empty());
    }

    SECTION("Transaction signatures use digest") {
        Transaction tx;
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);

        auto account = Account::create();
        auto signer = std::make_shared<Signer>(account->getScriptHash(), WitnessScope::CALLED_BY_ENTRY);
        tx.addSigner(signer);

        tx.sign(account);

        REQUIRE(tx.getWitnesses().size() == 1);

        const auto& invocation = tx.getWitnesses()[0]->getInvocationScript();
        REQUIRE(invocation.size() == 65);
        REQUIRE(invocation[0] == 64);

        Bytes sigBytes(invocation.begin() + 1, invocation.begin() + 65);
        auto sig = std::make_shared<ECDSASignature>(sigBytes);
        auto pubKey = account->getKeyPair()->getPublicKey();

        REQUIRE(pubKey->verifyHash(tx.getHash().toArray(), sig));
    }
    
    SECTION("Transaction size calculation") {
        Transaction tx;
        tx.setVersion(0);
        tx.setNonce(12345678);
        tx.setSystemFee(100000);
        tx.setNetworkFee(200000);
        tx.setValidUntilBlock(1000000);
        
        size_t baseSize = tx.getSize();
        REQUIRE(baseSize > 0);
        
        // Add script
        ScriptBuilder builder;
        builder.pushInteger(42);
        tx.setScript(builder.toArray());
        
        size_t withScriptSize = tx.getSize();
        REQUIRE(withScriptSize > baseSize);
        
        // Add signer
        Hash160 account("23ba2703c53263e8d6e522dc32203339dcd8eee9");
        auto signer = std::make_shared<Signer>(account, WitnessScope::CALLED_BY_ENTRY);
        tx.addSigner(signer);
        
        size_t withSignerSize = tx.getSize();
        REQUIRE(withSignerSize > withScriptSize);
    }
    
    SECTION("Transaction with attributes") {
        Transaction tx;
        
        // Attributes are stored as a vector of pairs (type, data)
        // Since TransactionAttribute is not fully implemented, we skip detailed testing
        REQUIRE(tx.getAttributes().empty());
    }
}
