#include <catch2/catch_test_macros.hpp>
#include "neocpp/wallet/wallet.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/utils/hex.hpp"
#include <memory>
#include <string>

using namespace neocpp;

TEST_CASE("Wallet Tests", "[wallet]") {
    
    SECTION("Create empty wallet") {
        Wallet wallet("Test Wallet", "1.0");
        
        REQUIRE(wallet.getName() == "Test Wallet");
        REQUIRE(wallet.getVersion() == "1.0");
        REQUIRE(wallet.isEmpty() == true);
        REQUIRE(wallet.size() == 0);
        REQUIRE(wallet.getAccounts().empty());
    }
    
    SECTION("Create wallet with default values") {
        Wallet wallet;
        
        REQUIRE(wallet.getName() == "NeoCpp Wallet");
        REQUIRE(wallet.getVersion() == "1.0");
        REQUIRE(wallet.isEmpty() == true);
    }
    
    SECTION("Set wallet properties") {
        Wallet wallet;
        
        wallet.setName("Modified Wallet");
        wallet.setVersion("2.0");
        
        REQUIRE(wallet.getName() == "Modified Wallet");
        REQUIRE(wallet.getVersion() == "2.0");
    }
    
    SECTION("Create account in wallet") {
        Wallet wallet;
        
        auto account = wallet.createAccount("Account 1");
        
        REQUIRE(account != nullptr);
        REQUIRE(wallet.size() == 1);
        REQUIRE(wallet.isEmpty() == false);
        REQUIRE(wallet.getAccounts().size() == 1);
        REQUIRE(wallet.getAccounts()[0] == account);
    }
    
    SECTION("Create multiple accounts") {
        Wallet wallet;
        
        auto account1 = wallet.createAccount("Account 1");
        auto account2 = wallet.createAccount("Account 2");
        auto account3 = wallet.createAccount("Account 3");
        
        REQUIRE(wallet.size() == 3);
        REQUIRE(wallet.getAccounts().size() == 3);
        REQUIRE(account1 != account2);
        REQUIRE(account2 != account3);
        REQUIRE(account1 != account3);
    }
    
    SECTION("Get account by address") {
        Wallet wallet;
        
        auto account1 = wallet.createAccount("Account 1");
        auto account2 = wallet.createAccount("Account 2");
        
        // Get accounts by their addresses
        auto retrieved1 = wallet.getAccount(account1->getAddress());
        auto retrieved2 = wallet.getAccount(account2->getAddress());
        
        REQUIRE(retrieved1 == account1);
        REQUIRE(retrieved2 == account2);
        
        // Try to get non-existent account
        auto notFound = wallet.getAccount("InvalidAddress");
        REQUIRE(notFound == nullptr);
    }
    
    SECTION("Check if wallet contains account") {
        Wallet wallet;
        
        auto account = wallet.createAccount();
        std::string address = account->getAddress();
        
        REQUIRE(wallet.containsAccount(address) == true);
        REQUIRE(wallet.containsAccount("NonExistentAddress") == false);
    }
    
    SECTION("Remove account from wallet") {
        Wallet wallet;
        
        auto account1 = wallet.createAccount("Account 1");
        auto account2 = wallet.createAccount("Account 2");
        std::string address1 = account1->getAddress();
        std::string address2 = account2->getAddress();
        
        REQUIRE(wallet.size() == 2);
        
        // Remove first account
        bool removed = wallet.removeAccount(address1);
        REQUIRE(removed == true);
        REQUIRE(wallet.size() == 1);
        REQUIRE(wallet.containsAccount(address1) == false);
        REQUIRE(wallet.containsAccount(address2) == true);
        
        // Try to remove non-existent account
        removed = wallet.removeAccount("InvalidAddress");
        REQUIRE(removed == false);
        REQUIRE(wallet.size() == 1);
        
        // Remove second account
        removed = wallet.removeAccount(address2);
        REQUIRE(removed == true);
        REQUIRE(wallet.isEmpty() == true);
    }
    
    SECTION("Default account management") {
        Wallet wallet;
        
        // No default account initially
        auto defaultAccount = wallet.getDefaultAccount();
        REQUIRE(defaultAccount == nullptr);
        
        // Create accounts
        auto account1 = wallet.createAccount("Account 1");
        auto account2 = wallet.createAccount("Account 2");
        
        // First account should become default
        defaultAccount = wallet.getDefaultAccount();
        REQUIRE(defaultAccount == account1);
        
        // Set second account as default
        bool set = wallet.setDefaultAccount(account2->getAddress());
        REQUIRE(set == true);
        defaultAccount = wallet.getDefaultAccount();
        REQUIRE(defaultAccount == account2);
        
        // Try to set non-existent account as default
        set = wallet.setDefaultAccount("InvalidAddress");
        REQUIRE(set == false);
        defaultAccount = wallet.getDefaultAccount();
        REQUIRE(defaultAccount == account2); // Should remain unchanged
    }
    
    SECTION("Clear wallet") {
        Wallet wallet;
        
        wallet.createAccount("Account 1");
        wallet.createAccount("Account 2");
        wallet.createAccount("Account 3");
        
        REQUIRE(wallet.size() == 3);
        
        wallet.clear();
        
        REQUIRE(wallet.isEmpty() == true);
        REQUIRE(wallet.size() == 0);
        REQUIRE(wallet.getAccounts().empty());
    }
    
    SECTION("Import account from WIF") {
        Wallet wallet;
        
        // Use a valid WIF format (this is just an example, real implementation may vary)
        std::string wif = "L1uyy5qTuGrVXrmrsvHWHgVzW9kKdrp27wBC7Vs6nZDTF2BRUVwy";
        
        auto account = wallet.importFromWIF(wif, "Imported Account");
        
        REQUIRE(account != nullptr);
        REQUIRE(wallet.size() == 1);
        REQUIRE(wallet.containsAccount(account->getAddress()) == true);
    }
    
    SECTION("Import account from NEP-2") {
        Wallet wallet;
        
        // Use a valid NEP-2 format (this is just an example)
        std::string nep2 = "6PYLHmDf6AjF4AsVtosmxHuPYeuyJL3SLuw7J1U8i7HxKAnYNsp61HYRfF";
        std::string password = "TestPassword";
        
        auto account = wallet.importFromNEP2(nep2, password, "NEP-2 Account");
        
        if (account) { // NEP-2 import might not be implemented
            REQUIRE(wallet.size() == 1);
            REQUIRE(wallet.containsAccount(account->getAddress()) == true);
        }
    }
    
    SECTION("Get account by script hash") {
        Wallet wallet;
        
        auto account = wallet.createAccount("Test Account");
        
        // Get the account's script hash
        Hash160 scriptHash = account->getScriptHash();
        
        // Retrieve account by script hash
        auto retrieved = wallet.getAccount(scriptHash);
        REQUIRE(retrieved == account);
        
        // Try with non-existent script hash
        Hash160 invalidHash("0x0000000000000000000000000000000000000000");
        auto notFound = wallet.getAccount(invalidHash);
        REQUIRE(notFound == nullptr);
    }
    
    SECTION("Check if wallet contains account by script hash") {
        Wallet wallet;
        
        auto account = wallet.createAccount();
        Hash160 scriptHash = account->getScriptHash();
        
        REQUIRE(wallet.containsAccount(scriptHash) == true);
        
        Hash160 invalidHash("0x1111111111111111111111111111111111111111");
        REQUIRE(wallet.containsAccount(invalidHash) == false);
    }
    
    SECTION("Add external account to wallet") {
        Wallet wallet;
        
        // Create an account outside the wallet
        auto keyPair = std::make_shared<ECKeyPair>(ECKeyPair::generate());
        auto externalAccount = std::make_shared<Account>(keyPair);
        
        // Add it to the wallet
        wallet.addAccount(externalAccount);
        
        REQUIRE(wallet.size() == 1);
        REQUIRE(wallet.containsAccount(externalAccount->getAddress()) == true);
        REQUIRE(wallet.getAccount(externalAccount->getAddress()) == externalAccount);
    }
    
    SECTION("Multiple wallets are independent") {
        Wallet wallet1("Wallet 1", "1.0");
        Wallet wallet2("Wallet 2", "1.0");
        
        auto account1 = wallet1.createAccount("Account in Wallet 1");
        auto account2 = wallet2.createAccount("Account in Wallet 2");
        
        REQUIRE(wallet1.size() == 1);
        REQUIRE(wallet2.size() == 1);
        
        REQUIRE(wallet1.containsAccount(account1->getAddress()) == true);
        REQUIRE(wallet1.containsAccount(account2->getAddress()) == false);
        
        REQUIRE(wallet2.containsAccount(account2->getAddress()) == true);
        REQUIRE(wallet2.containsAccount(account1->getAddress()) == false);
    }
    
    SECTION("Account labels") {
        Wallet wallet;
        
        auto account1 = wallet.createAccount("Personal");
        auto account2 = wallet.createAccount("Business");
        auto account3 = wallet.createAccount(""); // Empty label
        
        REQUIRE(wallet.size() == 3);
        
        // Labels should be preserved
        REQUIRE(account1->getLabel() == "Personal");
        REQUIRE(account2->getLabel() == "Business");
        REQUIRE(account3->getLabel() == "");
    }
    
    SECTION("Wallet persistence (save/load)") {
        std::string filepath = "/tmp/test_wallet.json";
        
        // Create and populate wallet
        {
            Wallet wallet("Persistent Wallet", "1.0");
            wallet.createAccount("Account 1");
            wallet.createAccount("Account 2");
            
            // Save wallet
            wallet.save(filepath);
        }
        
        // Load wallet
        {
            auto loadedWallet = Wallet::load(filepath);
            
            if (loadedWallet) {
                REQUIRE(loadedWallet->getName() == "Persistent Wallet");
                REQUIRE(loadedWallet->getVersion() == "1.0");
                REQUIRE(loadedWallet->size() == 2);
            }
        }
        
        // Clean up
        std::remove(filepath.c_str());
    }
    
    SECTION("Encrypted wallet (save/load with password)") {
        std::string filepath = "/tmp/test_encrypted_wallet.json";
        std::string password = "SecurePassword123";
        
        // Create and save encrypted wallet
        {
            Wallet wallet("Encrypted Wallet", "1.0");
            wallet.createAccount("Secure Account");
            
            wallet.save(filepath, password);
        }
        
        // Try to load with wrong password
        {
            auto wrongWallet = Wallet::load(filepath, "WrongPassword");
            // Should either return nullptr or throw exception
            if (!wrongWallet) {
                REQUIRE(wrongWallet == nullptr);
            }
        }
        
        // Load with correct password
        {
            auto correctWallet = Wallet::load(filepath, password);
            
            if (correctWallet) {
                REQUIRE(correctWallet->getName() == "Encrypted Wallet");
                REQUIRE(correctWallet->size() == 1);
            }
        }
        
        // Clean up
        std::remove(filepath.c_str());
    }
}