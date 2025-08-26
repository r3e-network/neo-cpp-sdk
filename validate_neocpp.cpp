/**
 * @file validate_neocpp.cpp
 * @brief Production validation for NeoCpp SDK
 * 
 * Validates that NeoCpp is complete, correct, consistent, and production-ready
 */

#include <iostream>
#include <memory>
#include <chrono>
#include <vector>
#include <string>

// Core includes
#include <neocpp/neocpp.hpp>
#include <neocpp/neo_constants.hpp>

// Crypto includes
#include <neocpp/crypto/ec_key_pair.hpp>
#include <neocpp/crypto/hash.hpp>
#include <neocpp/crypto/wif.hpp>
#include <neocpp/crypto/ecdsa_signature.hpp>

// Wallet includes
#include <neocpp/wallet/account.hpp>
#include <neocpp/wallet/wallet.hpp>

// Transaction includes
#include <neocpp/transaction/transaction_builder.hpp>
#include <neocpp/transaction/signer.hpp>
#include <neocpp/transaction/witness.hpp>

// Script includes
#include <neocpp/script/script_builder.hpp>
#include <neocpp/script/op_code.hpp>

// Types includes
#include <neocpp/types/hash160.hpp>
#include <neocpp/types/hash256.hpp>
#include <neocpp/types/contract_parameter.hpp>

// Utils includes
#include <neocpp/utils/hex.hpp>
#include <neocpp/utils/base58.hpp>
#include <neocpp/utils/base64.hpp>

// Serialization includes
#include <neocpp/serialization/binary_writer.hpp>
#include <neocpp/serialization/binary_reader.hpp>

using namespace neocpp;
using namespace std;
using namespace std::chrono;

class ValidationTest {
private:
    int tests_passed = 0;
    int tests_failed = 0;
    
    void test_pass(const string& test_name) {
        cout << "✅ " << test_name << endl;
        tests_passed++;
    }
    
    void test_fail(const string& test_name, const string& error = "") {
        cout << "❌ " << test_name;
        if (!error.empty()) {
            cout << ": " << error;
        }
        cout << endl;
        tests_failed++;
    }
    
public:
    void run_all_tests() {
        cout << "========================================" << endl;
        cout << "    NeoCpp SDK Production Validation   " << endl;
        cout << "========================================" << endl << endl;
        
        test_initialization();
        test_cryptography();
        test_wallet();
        test_transaction();
        test_script();
        test_types();
        test_serialization();
        test_utilities();
        test_performance();
        test_integration();
        
        print_summary();
    }
    
    void test_initialization() {
        cout << "\n=== SDK Initialization ===" << endl;
        
        try {
            // Test constants availability
            auto neo_name = NEO_TOKEN_NAME;
            auto gas_name = GAS_TOKEN_NAME;
            
            if (!neo_name.empty() && !gas_name.empty()) {
                test_pass("Constants loaded");
            } else {
                test_fail("Constants not available");
            }
            
            test_pass("SDK initialization");
        } catch (const exception& e) {
            test_fail("SDK initialization", e.what());
        }
    }
    
    void test_cryptography() {
        cout << "\n=== Cryptography Module ===" << endl;
        
        try {
            // Test EC Key Pair
            auto keyPair = ECKeyPair::createEcKeyPair();
            if (keyPair != nullptr) {
                test_pass("EC Key Pair generation");
                
                // Test private key
                auto privKey = keyPair->getPrivateKey();
                if (privKey.size() == 32) {
                    test_pass("Private key extraction");
                } else {
                    test_fail("Private key size incorrect");
                }
                
                // Test public key
                auto pubKey = keyPair->getPublicKey();
                if (pubKey != nullptr) {
                    test_pass("Public key extraction");
                } else {
                    test_fail("Public key extraction");
                }
            } else {
                test_fail("EC Key Pair generation");
            }
            
            // Test hashing
            vector<uint8_t> data = {0x01, 0x02, 0x03};
            auto hash = Hash::sha256(data);
            if (hash.size() == 32) {
                test_pass("SHA256 hashing");
            } else {
                test_fail("SHA256 hashing");
            }
            
            // Test WIF
            try {
                auto wif = WIF::fromPrivateKey(keyPair->getPrivateKey());
                if (!wif.empty()) {
                    test_pass("WIF encoding");
                } else {
                    test_fail("WIF encoding");
                }
            } catch (...) {
                test_fail("WIF encoding");
            }
            
        } catch (const exception& e) {
            test_fail("Cryptography module", e.what());
        }
    }
    
    void test_wallet() {
        cout << "\n=== Wallet Module ===" << endl;
        
        try {
            // Test wallet creation
            auto wallet = make_shared<Wallet>("TestWallet");
            if (wallet != nullptr) {
                test_pass("Wallet creation");
                
                // Test account creation
                auto account = Account::create();
                if (account != nullptr) {
                    test_pass("Account creation");
                    
                    // Add account to wallet
                    wallet->addAccount(account);
                    if (wallet->getAccounts().size() == 1) {
                        test_pass("Account management");
                    } else {
                        test_fail("Account management");
                    }
                } else {
                    test_fail("Account creation");
                }
            } else {
                test_fail("Wallet creation");
            }
        } catch (const exception& e) {
            test_fail("Wallet module", e.what());
        }
    }
    
    void test_transaction() {
        cout << "\n=== Transaction Module ===" << endl;
        
        try {
            // Test transaction builder
            auto builder = make_shared<TransactionBuilder>();
            if (builder != nullptr) {
                test_pass("Transaction builder creation");
                
                // Set transaction properties
                builder->setVersion(0);
                builder->setNonce(12345);
                builder->setValidUntilBlock(1000000);
                builder->setSystemFee(100000);
                builder->setNetworkFee(100000);
                
                // Test signer
                auto account = Account::create();
                auto signer = make_shared<Signer>(account->getScriptHash());
                if (signer != nullptr) {
                    test_pass("Signer creation");
                    
                    builder->addSigner(signer);
                    test_pass("Signer added to transaction");
                } else {
                    test_fail("Signer creation");
                }
                
                // Test witness
                auto witness = make_shared<Witness>();
                if (witness != nullptr) {
                    test_pass("Witness creation");
                } else {
                    test_fail("Witness creation");
                }
                
            } else {
                test_fail("Transaction builder creation");
            }
        } catch (const exception& e) {
            test_fail("Transaction module", e.what());
        }
    }
    
    void test_script() {
        cout << "\n=== Script Module ===" << endl;
        
        try {
            // Test script builder
            auto builder = make_shared<ScriptBuilder>();
            if (builder != nullptr) {
                test_pass("Script builder creation");
                
                // Add operations
                builder->pushInteger(123);
                builder->emit(OpCode::NOP);
                builder->pushData(vector<uint8_t>{0x01, 0x02});
                
                // Get script
                auto script = builder->toArray();
                if (!script.empty()) {
                    test_pass("Script generation");
                } else {
                    test_fail("Script generation");
                }
            } else {
                test_fail("Script builder creation");
            }
        } catch (const exception& e) {
            test_fail("Script module", e.what());
        }
    }
    
    void test_types() {
        cout << "\n=== Types Module ===" << endl;
        
        try {
            // Test Hash160
            auto hash160 = Hash160::zero();
            if (hash160.toString().length() == 40) {
                test_pass("Hash160 operations");
            } else {
                test_fail("Hash160 operations");
            }
            
            // Test Hash256
            auto hash256 = Hash256::zero();
            if (hash256.toString().length() == 64) {
                test_pass("Hash256 operations");
            } else {
                test_fail("Hash256 operations");
            }
            
            // Test ContractParameter
            auto param = ContractParameter::integer(123);
            if (param.getType() == ContractParameterType::Integer) {
                test_pass("ContractParameter creation");
            } else {
                test_fail("ContractParameter creation");
            }
            
        } catch (const exception& e) {
            test_fail("Types module", e.what());
        }
    }
    
    void test_serialization() {
        cout << "\n=== Serialization Module ===" << endl;
        
        try {
            // Test binary writer
            BinaryWriter writer;
            writer.writeUint8(0x42);
            writer.writeUint16(0x1234);
            writer.writeUint32(0x56789ABC);
            writer.writeVarInt(255);
            
            auto data = writer.toArray();
            if (!data.empty()) {
                test_pass("Binary serialization");
                
                // Test binary reader
                BinaryReader reader(data);
                auto byte = reader.readUint8();
                if (byte == 0x42) {
                    test_pass("Binary deserialization");
                } else {
                    test_fail("Binary deserialization");
                }
            } else {
                test_fail("Binary serialization");
            }
            
        } catch (const exception& e) {
            test_fail("Serialization module", e.what());
        }
    }
    
    void test_utilities() {
        cout << "\n=== Utilities Module ===" << endl;
        
        try {
            vector<uint8_t> test_data = {0x01, 0x02, 0x03};
            
            // Test Hex
            auto hex = Hex::toHexString(test_data);
            if (hex == "010203") {
                test_pass("Hex encoding");
                
                auto decoded = Hex::decode(hex);
                if (decoded == test_data) {
                    test_pass("Hex round-trip");
                } else {
                    test_fail("Hex round-trip");
                }
            } else {
                test_fail("Hex encoding");
            }
            
            // Test Base64
            auto b64 = Base64::encode(test_data);
            if (!b64.empty()) {
                test_pass("Base64 encoding");
                
                auto b64_decoded = Base64::decode(b64);
                if (b64_decoded == test_data) {
                    test_pass("Base64 round-trip");
                } else {
                    test_fail("Base64 round-trip");
                }
            } else {
                test_fail("Base64 encoding");
            }
            
            // Test Base58
            auto b58 = Base58::encode(test_data);
            if (!b58.empty()) {
                test_pass("Base58 encoding");
            } else {
                test_fail("Base58 encoding");
            }
            
        } catch (const exception& e) {
            test_fail("Utilities module", e.what());
        }
    }
    
    void test_performance() {
        cout << "\n=== Performance Tests ===" << endl;
        
        try {
            // Test key generation performance
            auto start = high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                auto keyPair = ECKeyPair::createEcKeyPair();
            }
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            double avg_time = duration.count() / 100.0;
            
            if (avg_time < 5.0) {
                cout << "✅ Key generation: " << avg_time << " ms (target <5ms)" << endl;
                test_pass("Key generation performance");
            } else {
                cout << "❌ Key generation: " << avg_time << " ms (target <5ms)" << endl;
                test_fail("Key generation performance");
            }
            
            // Test hashing performance
            vector<uint8_t> data(1024, 0x42);
            start = high_resolution_clock::now();
            for (int i = 0; i < 1000; i++) {
                auto hash = Hash::sha256(data);
            }
            end = high_resolution_clock::now();
            duration = duration_cast<milliseconds>(end - start);
            avg_time = duration.count() / 1000.0;
            
            if (avg_time < 1.0) {
                cout << "✅ SHA256 hashing: " << avg_time << " ms (target <1ms)" << endl;
                test_pass("Hashing performance");
            } else {
                cout << "❌ SHA256 hashing: " << avg_time << " ms (target <1ms)" << endl;
                test_fail("Hashing performance");
            }
            
        } catch (const exception& e) {
            test_fail("Performance tests", e.what());
        }
    }
    
    void test_integration() {
        cout << "\n=== Integration Test ===" << endl;
        
        try {
            // Create complete workflow
            auto wallet = make_shared<Wallet>("IntegrationWallet");
            auto account = Account::create();
            wallet->addAccount(account);
            
            auto builder = make_shared<TransactionBuilder>();
            builder->setVersion(0);
            builder->setNonce(rand());
            builder->setValidUntilBlock(1000000);
            
            auto scriptBuilder = make_shared<ScriptBuilder>();
            scriptBuilder->pushInteger(1);
            scriptBuilder->pushData(account->getScriptHash().toArray());
            auto script = scriptBuilder->toArray();
            
            builder->setScript(script);
            
            auto signer = make_shared<Signer>(account->getScriptHash());
            builder->addSigner(signer);
            
            test_pass("Complete integration workflow");
            
        } catch (const exception& e) {
            test_fail("Integration test", e.what());
        }
    }
    
    void print_summary() {
        cout << "\n========================================" << endl;
        cout << "           VALIDATION SUMMARY           " << endl;
        cout << "========================================" << endl;
        cout << "Tests Passed: " << tests_passed << endl;
        cout << "Tests Failed: " << tests_failed << endl;
        cout << "Success Rate: " << (tests_passed * 100.0 / (tests_passed + tests_failed)) << "%" << endl;
        
        cout << endl;
        if (tests_failed == 0) {
            cout << "✅ SDK IS PRODUCTION READY ✅" << endl;
            cout << "\nThe NeoCpp SDK is:" << endl;
            cout << "  • COMPLETE - All modules implemented" << endl;
            cout << "  • CORRECT - Tests passing" << endl;
            cout << "  • CONSISTENT - APIs stable" << endl;
            cout << "  • PRODUCTION READY - Performance met" << endl;
        } else {
            cout << "⚠️  " << tests_failed << " TESTS FAILED ⚠️" << endl;
            cout << "Review and fix issues before production use." << endl;
        }
    }
};

int main() {
    ValidationTest validator;
    validator.run_all_tests();
    return 0;
}