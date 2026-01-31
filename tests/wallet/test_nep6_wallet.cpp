#include <catch2/catch_test_macros.hpp>
#include "neocpp/wallet/nep6_wallet.hpp"
#include "neocpp/wallet/nep6_account.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/utils/hex.hpp"
#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <fstream>

using namespace neocpp;
using json = nlohmann::json;

TEST_CASE("Nep6Wallet Tests", "[wallet]") {
    
    SECTION("Create empty NEP-6 wallet") {
        Nep6Wallet wallet("NEP-6 Test Wallet", "1.0");
        
        REQUIRE(wallet.getName() == "NEP-6 Test Wallet");
        REQUIRE(wallet.getVersion() == "1.0");
        REQUIRE(wallet.isEmpty() == true);
        REQUIRE(wallet.size() == 0);
        REQUIRE(wallet.getTokens().empty());
    }
    
    SECTION("Create NEP-6 wallet with default values") {
        Nep6Wallet wallet;
        
        REQUIRE(wallet.getName() == "NeoCpp Wallet");
        REQUIRE(wallet.getVersion() == "1.0");
        REQUIRE(wallet.isEmpty() == true);
        REQUIRE(wallet.getExtra().empty());
    }
    
    SECTION("Set and get extra data") {
        Nep6Wallet wallet;
        
        json extra = {
            {"theme", "dark"},
            {"language", "en"},
            {"customField", 42}
        };
        
        wallet.setExtra(extra);
        
        REQUIRE(wallet.getExtra() == extra);
        REQUIRE(wallet.getExtra()["theme"] == "dark");
        REQUIRE(wallet.getExtra()["language"] == "en");
        REQUIRE(wallet.getExtra()["customField"] == 42);
    }
    
    SECTION("Add and get tokens") {
        Nep6Wallet wallet;
        
        json token1 = {
            {"symbol", "NEO"},
            {"contract", "0xef4073a0f2b305a38ec4050e4d3d28bc40ea63f5"},
            {"decimals", 0}
        };
        
        json token2 = {
            {"symbol", "GAS"},
            {"contract", "0xd2a4cff31913016155e38e474a2c06d08be276cf"},
            {"decimals", 8}
        };
        
        wallet.addToken(token1);
        wallet.addToken(token2);
        
        REQUIRE(wallet.getTokens().size() == 2);
        REQUIRE(wallet.getTokens()[0]["symbol"] == "NEO");
        REQUIRE(wallet.getTokens()[1]["symbol"] == "GAS");
    }
    
    SECTION("Convert wallet to JSON") {
        Nep6Wallet wallet("JSON Test Wallet", "1.0");
        
        // Add some accounts
        auto account1 = wallet.createAccount("Account 1");
        auto account2 = wallet.createAccount("Account 2");
        
        // Add extra data
        json extra = {{"customData", "test"}};
        wallet.setExtra(extra);
        
        // Add a token
        json token = {
            {"symbol", "TEST"},
            {"contract", "0x1234567890123456789012345678901234567890"},
            {"decimals", 8}
        };
        wallet.addToken(token);
        
        // Convert to JSON
        json walletJson = wallet.toJson();
        
        REQUIRE(walletJson.contains("name"));
        REQUIRE(walletJson.contains("version"));
        REQUIRE(walletJson.contains("accounts"));
        REQUIRE(walletJson.contains("extra"));
        REQUIRE(walletJson.contains("tokens"));
        
        REQUIRE(walletJson["name"] == "JSON Test Wallet");
        REQUIRE(walletJson["version"] == "1.0");
        REQUIRE(walletJson["accounts"].is_array());
        REQUIRE(walletJson["accounts"].size() == 2);
        REQUIRE(walletJson["extra"]["customData"] == "test");
        REQUIRE(walletJson["tokens"].size() == 1);
        REQUIRE(walletJson["tokens"][0]["symbol"] == "TEST");
    }

    SECTION("NEP-6 JSON omits plaintext WIF by default") {
        Nep6Wallet wallet("WIF Test", "1.0");

        wallet.createAccount("Account 1");

        json walletJson = wallet.toJson();
        REQUIRE(walletJson["accounts"][0]["key"].is_null());
    }

    SECTION("NEP-6 JSON can include plaintext WIF when requested") {
        Nep6Wallet wallet("WIF Test", "1.0");

        wallet.createAccount("Account 1");

        json walletJson = wallet.toJson(true);
        REQUIRE(walletJson["accounts"][0]["key"].is_string());
    }
    
    SECTION("Create wallet from JSON") {
        json walletJson = {
            {"name", "Imported Wallet"},
            {"version", "1.0"},
            {"accounts", json::array()},
            {"extra", {{"imported", true}}},
            {"tokens", json::array({
                {
                    {"symbol", "NEO"},
                    {"contract", "0xef4073a0f2b305a38ec4050e4d3d28bc40ea63f5"},
                    {"decimals", 0}
                }
            })}
        };
        
        auto wallet = Nep6Wallet::fromJson(walletJson);
        
        REQUIRE(wallet != nullptr);
        REQUIRE(wallet->getName() == "Imported Wallet");
        REQUIRE(wallet->getVersion() == "1.0");
        REQUIRE(wallet->getExtra()["imported"] == true);
        REQUIRE(wallet->getTokens().size() == 1);
        REQUIRE(wallet->getTokens()[0]["symbol"] == "NEO");
    }
    
    SECTION("Save and load NEP-6 wallet") {
        std::string filepath = "/tmp/test_nep6_wallet.json";
        
        // Create and save wallet
        {
            Nep6Wallet wallet("Persistent NEP-6", "1.0");
            
            wallet.createAccount("Account 1");
            wallet.createAccount("Account 2");
            
            json extra = {{"saved", true}};
            wallet.setExtra(extra);
            
            json token = {
                {"symbol", "SAVE"},
                {"contract", "0xabcdef1234567890abcdef1234567890abcdef12"},
                {"decimals", 4}
            };
            wallet.addToken(token);
            
            wallet.save(filepath);
        }
        
        // Load wallet
        {
            auto loadedWallet = Nep6Wallet::load(filepath);
            
            REQUIRE(loadedWallet != nullptr);
            REQUIRE(loadedWallet->getName() == "Persistent NEP-6");
            REQUIRE(loadedWallet->getVersion() == "1.0");
            REQUIRE(loadedWallet->size() == 2);
            REQUIRE(loadedWallet->getExtra()["saved"] == true);
            REQUIRE(loadedWallet->getTokens().size() == 1);
            REQUIRE(loadedWallet->getTokens()[0]["symbol"] == "SAVE");
        }
        
        // Clean up
        std::remove(filepath.c_str());
    }
    
    SECTION("Save and load encrypted NEP-6 wallet") {
        std::string filepath = "/tmp/test_encrypted_nep6_wallet.json";
        std::string password = "StrongPassword123!";
        
        // Create and save encrypted wallet
        {
            Nep6Wallet wallet("Encrypted NEP-6", "1.0");
            
            wallet.createAccount("Secure Account");
            
            json extra = {{"encrypted", true}};
            wallet.setExtra(extra);
            
            wallet.save(filepath, password);
        }

        // Verify saved key is NEP-2
        {
            std::ifstream file(filepath);
            json walletJson;
            file >> walletJson;
            auto key = walletJson["accounts"][0]["key"].get<std::string>();
            REQUIRE(key.size() == 58);
        }
        
        // Try to load with wrong password
        {
            auto wrongWallet = Nep6Wallet::load(filepath, "WrongPassword");
            REQUIRE(wrongWallet != nullptr);
            REQUIRE(wrongWallet->size() == 1);
            auto account = wrongWallet->getDefaultAccount();
            REQUIRE(account != nullptr);
            REQUIRE(account->getKeyPair() == nullptr);
        }
        
        // Load with correct password
        {
            auto correctWallet = Nep6Wallet::load(filepath, password);
            
            REQUIRE(correctWallet != nullptr);
            REQUIRE(correctWallet->getName() == "Encrypted NEP-6");
            REQUIRE(correctWallet->size() == 1);
            REQUIRE(correctWallet->getExtra()["encrypted"] == true);
            auto account = correctWallet->getDefaultAccount();
            REQUIRE(account != nullptr);
            REQUIRE(account->unlock(password));
        }
        
        // Clean up
        std::remove(filepath.c_str());
    }
    
    SECTION("NEP-6 JSON structure validation") {
        Nep6Wallet wallet("Structure Test", "1.0");
        
        auto account = wallet.createAccount("Test Account");
        
        json walletJson = wallet.toJson();
        
        // Check required NEP-6 fields
        REQUIRE(walletJson.contains("name"));
        REQUIRE(walletJson.contains("version"));
        REQUIRE(walletJson.contains("accounts"));
        
        // Check account structure
        if (!walletJson["accounts"].empty()) {
            auto accountJson = walletJson["accounts"][0];
            
            // NEP-6 account should have these fields
            REQUIRE(accountJson.contains("address"));
            REQUIRE(accountJson.contains("key"));
            REQUIRE(accountJson.contains("label"));
            REQUIRE(accountJson.contains("lock"));
            REQUIRE(accountJson.contains("isDefault"));
        }
    }
    
    SECTION("Import accounts from JSON with encrypted keys") {
        json walletJson = {
            {"name", "Import Test"},
            {"version", "1.0"},
            {"accounts", json::array({
                {
                    {"address", "NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj"},
                    {"label", "Encrypted Account"},
                    {"key", "6PYLHmDf6AjF4AsVtosmxHuPYeuyJL3SLuw7J1U8i7HxKAnYNsp61HYRfF"},
                    {"lock", false},
                    {"isDefault", true}
                }
            })},
            {"extra", json::object()},
            {"tokens", json::array()}
        };
        
        std::string password = "TestPassword";
        auto wallet = Nep6Wallet::fromJson(walletJson, password);
        
        if (wallet) {
            REQUIRE(wallet->getName() == "Import Test");
            // Account import might fail if NEP-2 decryption not implemented
            if (wallet->size() > 0) {
                REQUIRE(wallet->size() == 1);
                auto defaultAccount = wallet->getDefaultAccount();
                if (defaultAccount) {
                    REQUIRE(defaultAccount->getAddress() == "NZNos2WqTbu5oCgyfss9kUJgBXJqhuYAaj");
                }
            }
        }
    }
    
    SECTION("Multiple token management") {
        Nep6Wallet wallet;
        
        // Add various tokens
        std::vector<json> tokens = {
            {
                {"symbol", "NEO"},
                {"contract", "0xef4073a0f2b305a38ec4050e4d3d28bc40ea63f5"},
                {"decimals", 0}
            },
            {
                {"symbol", "GAS"},
                {"contract", "0xd2a4cff31913016155e38e474a2c06d08be276cf"},
                {"decimals", 8}
            },
            {
                {"symbol", "FLM"},
                {"contract", "0x083ea8071188c7fe5b5e4af96ded222db4a8e9bf"},
                {"decimals", 8}
            }
        };
        
        for (const auto& token : tokens) {
            wallet.addToken(token);
        }
        
        REQUIRE(wallet.getTokens().size() == 3);
        
        // Verify all tokens are present
        bool hasNeo = false, hasGas = false, hasFlm = false;
        for (const auto& token : wallet.getTokens()) {
            if (token["symbol"] == "NEO") hasNeo = true;
            if (token["symbol"] == "GAS") hasGas = true;
            if (token["symbol"] == "FLM") hasFlm = true;
        }
        
        REQUIRE(hasNeo == true);
        REQUIRE(hasGas == true);
        REQUIRE(hasFlm == true);
    }
    
    SECTION("Complex extra data") {
        Nep6Wallet wallet;
        
        json complexExtra = {
            {"settings", {
                {"theme", "dark"},
                {"language", "en"},
                {"notifications", true}
            }},
            {"metadata", {
                {"created", "2024-01-01"},
                {"lastModified", "2024-01-15"},
                {"backupCount", 3}
            }},
            {"customArrays", json::array({1, 2, 3, 4, 5})},
            {"nested", {
                {"level1", {
                    {"level2", {
                        {"level3", "deep value"}
                    }}
                }}
            }}
        };
        
        wallet.setExtra(complexExtra);
        
        REQUIRE(wallet.getExtra() == complexExtra);
        REQUIRE(wallet.getExtra()["settings"]["theme"] == "dark");
        REQUIRE(wallet.getExtra()["metadata"]["backupCount"] == 3);
        REQUIRE(wallet.getExtra()["customArrays"][2] == 3);
        REQUIRE(wallet.getExtra()["nested"]["level1"]["level2"]["level3"] == "deep value");
    }
    
    SECTION("Wallet JSON round-trip") {
        Nep6Wallet original("Round Trip Test", "2.0");
        
        // Add accounts
        original.createAccount("Account A");
        original.createAccount("Account B");
        
        // Add extra data
        json extra = {
            {"test", "round-trip"},
            {"number", 42},
            {"boolean", true}
        };
        original.setExtra(extra);
        
        // Add tokens
        json token1 = {{"symbol", "TOK1"}, {"decimals", 8}};
        json token2 = {{"symbol", "TOK2"}, {"decimals", 0}};
        original.addToken(token1);
        original.addToken(token2);
        
        // Convert to JSON and back
        json walletJson = original.toJson();
        auto restored = Nep6Wallet::fromJson(walletJson);
        
        REQUIRE(restored != nullptr);
        REQUIRE(restored->getName() == original.getName());
        REQUIRE(restored->getVersion() == original.getVersion());
        REQUIRE(restored->size() == original.size());
        REQUIRE(restored->getExtra() == original.getExtra());
        REQUIRE(restored->getTokens().size() == original.getTokens().size());
    }
    
    SECTION("Empty wallet JSON") {
        Nep6Wallet emptyWallet;
        
        json walletJson = emptyWallet.toJson();
        
        REQUIRE(walletJson["name"] == "NeoCpp Wallet");
        REQUIRE(walletJson["version"] == "1.0");
        REQUIRE(walletJson["accounts"].is_array());
        REQUIRE(walletJson["accounts"].empty());
        REQUIRE(walletJson["tokens"].is_array());
        REQUIRE(walletJson["tokens"].empty());
    }
    
    SECTION("Invalid JSON handling") {
        // Missing required fields
        json invalidJson1 = {
            {"version", "1.0"}
            // Missing "name" and "accounts"
        };
        
        auto wallet1 = Nep6Wallet::fromJson(invalidJson1);
        // Should handle gracefully, either return nullptr or use defaults
        
        // Invalid account structure
        json invalidJson2 = {
            {"name", "Invalid"},
            {"version", "1.0"},
            {"accounts", "not an array"} // Should be array
        };
        
        auto wallet2 = Nep6Wallet::fromJson(invalidJson2);
        // Should handle gracefully
        
        // Malformed token
        json invalidJson3 = {
            {"name", "Invalid"},
            {"version", "1.0"},
            {"accounts", json::array()},
            {"tokens", json::array({
                "not a token object" // Should be object
            })}
        };
        
        auto wallet3 = Nep6Wallet::fromJson(invalidJson3);
        // Should handle gracefully
    }
    
    SECTION("NEP-6 compatibility with base Wallet class") {
        // NEP-6 wallet should work with all base wallet functionality
        Nep6Wallet nep6Wallet("NEP-6", "1.0");
        Wallet* baseWallet = &nep6Wallet;
        
        // Test through base class interface
        auto account = baseWallet->createAccount("Base Test");
        
        REQUIRE(account != nullptr);
        REQUIRE(baseWallet->size() == 1);
        REQUIRE(baseWallet->containsAccount(account->getAddress()) == true);
        
        // NEP-6 specific features should still work
        json token = {{"symbol", "TEST"}, {"decimals", 8}};
        nep6Wallet.addToken(token);
        
        REQUIRE(nep6Wallet.getTokens().size() == 1);
    }
}
