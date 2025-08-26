#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "neocpp/transaction/transaction_builder.hpp"
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/account_signer.hpp"
#include "neocpp/transaction/contract_signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/hash256.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/neo_constants.hpp"
#include <memory>
#include <random>

using namespace neocpp;

class MockNeoRpcClient {
public:
    uint32_t blockCount = 1000;
    int64_t systemFee = 100000;
    int64_t networkFee = 50000;
    
    uint32_t getBlockCount() { return blockCount; }
    int64_t calculateSystemFee(const Bytes&) { return systemFee; }
    int64_t calculateNetworkFee(const SharedPtr<Transaction>&) { return networkFee; }
};

TEST_CASE("TransactionBuilder Complete Tests", "[transaction]") {
    
    // Test accounts
    auto account1 = Account::fromPrivateKey(
        Hex::decode("e6e919577dd7b8e97805151c05ae07ff4f752654d6d8797597aca989c02c4cb3"));
    auto account2 = Account::fromPrivateKey(
        Hex::decode("b4b2b579cac270125259f08a5f414e9235817e7637b9a66cfeb3b77d90c8e7f9"));
    
    Hash160 neoTokenHash("0xef4073a0f2b305a38ec4050e4d3d28bc40ea63f5");
    Hash160 gasTokenHash("0xd2a4cff31913016155e38e474a2c06d08be276cf");
    Hash160 recipient("0x969a77db482f74ce27105f760efa139223431394");
    
    auto mockClient = std::make_shared<MockNeoRpcClient>();
    
    SECTION("Build transaction with correct nonce") {
        TransactionBuilder builder(mockClient);
        
        // Test various nonce values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
        
        uint32_t nonce = dist(gen);
        builder.nonce(nonce)
               .validUntilBlock(1000)
               .script({0x01, 0x02, 0x03})
               .signers({AccountSigner::calledByEntry(account1)});
        
        auto tx = builder.build();
        REQUIRE(tx->getNonce() == nonce);
        
        // Test with 0
        builder.nonce(0);
        tx = builder.build();
        REQUIRE(tx->getNonce() == 0);
        
        // Test with max value
        builder.nonce(UINT32_MAX);
        tx = builder.build();
        REQUIRE(tx->getNonce() == UINT32_MAX);
    }
    
    SECTION("Fail building transaction with invalid nonce") {
        TransactionBuilder builder(mockClient);
        
        // C++ uses uint32_t, so negative values and values > UINT32_MAX 
        // are not possible at compile time
        // This test validates the type safety
        REQUIRE_NOTHROW(builder.nonce(0));
        REQUIRE_NOTHROW(builder.nonce(UINT32_MAX));
    }
    
    SECTION("Fail building transaction with invalid block number") {
        TransactionBuilder builder(mockClient);
        
        // Test with max value + 1 would overflow uint32_t
        REQUIRE_THROWS_WITH(
            builder.validUntilBlock(UINT32_MAX + 1ULL),
            Catch::Matchers::ContainsSubstring("Invalid block number")
        );
    }
    
    SECTION("Automatically set nonce") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x01, 0x02, 0x03})
               .signers({AccountSigner::calledByEntry(account1)});
        
        auto tx = builder.build();
        REQUIRE(tx->getNonce() > 0);
        REQUIRE(tx->getNonce() <= UINT32_MAX);
    }
    
    SECTION("Fail building transaction without any signer") {
        TransactionBuilder builder(mockClient);
        
        builder.validUntilBlock(100)
               .script({0x01, 0x02, 0x03});
        
        REQUIRE_THROWS_WITH(
            builder.build(),
            Catch::Matchers::ContainsSubstring("Cannot create a transaction without signers")
        );
    }
    
    SECTION("Override signer") {
        TransactionBuilder builder(mockClient);
        
        builder.signers({AccountSigner::global(account1)});
        REQUIRE(builder.getSigners().size() == 1);
        REQUIRE(builder.getSigners()[0]->getAccount() == account1);
        
        // Override with new signer
        builder.signers({AccountSigner::global(account2)});
        REQUIRE(builder.getSigners().size() == 1);
        REQUIRE(builder.getSigners()[0]->getAccount() == account2);
    }
    
    SECTION("Attributes high priority") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51}) // PUSH1
               .attribute(TransactionAttribute::highPriority())
               .signers({AccountSigner::none(account1)});
        
        auto tx = builder.build();
        REQUIRE(tx->getAttributes().size() == 1);
        REQUIRE(tx->getAttributes()[0].getType() == TransactionAttributeType::HIGH_PRIORITY);
    }
    
    SECTION("High priority only added once") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .attribute(TransactionAttribute::highPriority())
               .attribute(TransactionAttribute::highPriority())
               .signers({AccountSigner::none(account1)});
        
        auto tx = builder.build();
        REQUIRE(tx->getAttributes().size() == 1);
    }
    
    SECTION("Fail adding more than max attributes") {
        TransactionBuilder builder(mockClient);
        
        // Try to add more than max attributes
        for (int i = 0; i <= NeoConstants::MAX_TRANSACTION_ATTRIBUTES; i++) {
            if (i < NeoConstants::MAX_TRANSACTION_ATTRIBUTES) {
                REQUIRE_NOTHROW(builder.attribute(TransactionAttribute::highPriority()));
            } else {
                REQUIRE_THROWS_WITH(
                    builder.attribute(TransactionAttribute::highPriority()),
                    Catch::Matchers::ContainsSubstring("cannot have more than")
                );
            }
        }
    }
    
    SECTION("Automatic setting of valid until block") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::none(account1)});
        
        auto tx = builder.build();
        // Should be current block + max increment
        REQUIRE(tx->getValidUntilBlock() > mockClient->getBlockCount());
        REQUIRE(tx->getValidUntilBlock() <= mockClient->getBlockCount() + NeoConstants::MAX_VALID_UNTIL_BLOCK_INCREMENT);
    }
    
    SECTION("Automatic setting of system and network fees") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::none(account1)});
        
        auto tx = builder.build();
        REQUIRE(tx->getSystemFee() == mockClient->systemFee);
        REQUIRE(tx->getNetworkFee() == mockClient->networkFee);
    }
    
    SECTION("Sign transaction with account") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::calledByEntry(account1)});
        
        auto tx = builder.build();
        tx->sign(account1);
        
        REQUIRE(tx->getWitnesses().size() == 1);
        REQUIRE(!tx->getWitnesses()[0]->getInvocationScript().empty());
        REQUIRE(!tx->getWitnesses()[0]->getVerificationScript().empty());
    }
    
    SECTION("Fail signing with account missing private key") {
        auto accountNoKey = Account::fromAddress("NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj");
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::calledByEntry(accountNoKey)});
        
        auto tx = builder.build();
        REQUIRE_THROWS_WITH(
            tx->sign(accountNoKey),
            Catch::Matchers::ContainsSubstring("private key")
        );
    }
    
    SECTION("Contract witness") {
        Hash160 contractHash("0x1234567890123456789012345678901234567890");
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({ContractSigner::calledByEntry(contractHash)});
        
        auto tx = builder.build();
        // Contract signers don't require signatures
        REQUIRE(tx->getSigners().size() == 1);
        REQUIRE(tx->getSigners()[0]->getScriptHash() == contractHash);
    }
    
    SECTION("Transfer NEO from normal account") {
        TransactionBuilder builder(mockClient);
        
        ScriptBuilder scriptBuilder;
        scriptBuilder.contractCall(neoTokenHash, "transfer", {
            ContractParameter::hash160(account1->getScriptHash()),
            ContractParameter::hash160(recipient),
            ContractParameter::integer(10),
            ContractParameter::any()
        });
        
        builder.script(scriptBuilder.toArray())
               .signers({AccountSigner::calledByEntry(account1)});
        
        auto tx = builder.build();
        REQUIRE(!tx->getScript().empty());
        REQUIRE(tx->getSigners().size() == 1);
    }
    
    SECTION("Extend script") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51}); // PUSH1
        builder.extendScript({0x52}); // PUSH2
        builder.extendScript({0x53}); // PUSH3
        
        auto tx = builder.build();
        REQUIRE(tx->getScript() == Bytes{0x51, 0x52, 0x53});
    }
    
    SECTION("Additional network fee") {
        TransactionBuilder builder(mockClient);
        int64_t additionalFee = 1000000;
        
        builder.script({0x51})
               .signers({AccountSigner::none(account1)})
               .additionalNetworkFee(additionalFee);
        
        auto tx = builder.build();
        REQUIRE(tx->getNetworkFee() == mockClient->networkFee + additionalFee);
    }
    
    SECTION("Additional system fee") {
        TransactionBuilder builder(mockClient);
        int64_t additionalFee = 2000000;
        
        builder.script({0x51})
               .signers({AccountSigner::none(account1)})
               .additionalSystemFee(additionalFee);
        
        auto tx = builder.build();
        REQUIRE(tx->getSystemFee() == mockClient->systemFee + additionalFee);
    }
    
    SECTION("Set first signer") {
        TransactionBuilder builder(mockClient);
        
        builder.signers({
            AccountSigner::none(account2),
            AccountSigner::calledByEntry(account1)
        });
        
        // Set account1 as first signer
        builder.firstSigner(account1);
        
        auto signers = builder.getSigners();
        REQUIRE(signers.size() == 2);
        REQUIRE(signers[0]->getAccount() == account1);
        REQUIRE(signers[1]->getAccount() == account2);
    }
    
    SECTION("Get unsigned transaction") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::calledByEntry(account1)});
        
        auto tx = builder.getUnsignedTransaction();
        REQUIRE(tx->getWitnesses().empty());
        REQUIRE(!tx->getScript().empty());
        REQUIRE(tx->getSigners().size() == 1);
    }
    
    SECTION("Version") {
        TransactionBuilder builder(mockClient);
        
        builder.script({0x51})
               .signers({AccountSigner::none(account1)})
               .version(1);
        
        auto tx = builder.build();
        REQUIRE(tx->getVersion() == 1);
    }
    
    SECTION("Build with invalid script") {
        TransactionBuilder builder(mockClient);
        
        // Empty script should fail
        builder.signers({AccountSigner::none(account1)});
        
        REQUIRE_THROWS_WITH(
            builder.build(),
            Catch::Matchers::ContainsSubstring("script")
        );
    }
    
    SECTION("Multiple signers with same account different scopes") {
        TransactionBuilder builder(mockClient);
        
        // Should fail - same account with different scopes
        REQUIRE_THROWS_WITH(
            builder.signers({
                AccountSigner::global(account1),
                AccountSigner::calledByEntry(account1)
            }),
            Catch::Matchers::ContainsSubstring("same account")
        );
    }
    
    SECTION("Witness rules") {
        TransactionBuilder builder(mockClient);
        
        WitnessRule rule = WitnessRule::allowContracts(
            {Hash160("0x1234567890123456789012345678901234567890")}
        );
        
        auto signer = AccountSigner::none(account1);
        signer->setRules({rule});
        
        builder.script({0x51})
               .signers({signer});
        
        auto tx = builder.build();
        REQUIRE(tx->getSigners()[0]->getRules().size() == 1);
    }
    
    SECTION("Multiple signers ordering") {
        TransactionBuilder builder(mockClient);
        
        auto account3 = Account::create();
        
        builder.script({0x51})
               .signers({
                   AccountSigner::none(account2),
                   AccountSigner::none(account1),
                   AccountSigner::none(account3)
               });
        
        auto tx = builder.build();
        REQUIRE(tx->getSigners().size() == 3);
        // Verify order is preserved
        REQUIRE(tx->getSigners()[0]->getScriptHash() == account2->getScriptHash());
        REQUIRE(tx->getSigners()[1]->getScriptHash() == account1->getScriptHash());
        REQUIRE(tx->getSigners()[2]->getScriptHash() == account3->getScriptHash());
    }
}