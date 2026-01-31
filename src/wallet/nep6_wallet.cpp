#include "neocpp/wallet/nep6_wallet.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <fstream>
#include <iomanip>

namespace neocpp {

Nep6Wallet::Nep6Wallet(const std::string& name, const std::string& version)
    : Wallet(name, version) {
}

void Nep6Wallet::save(const std::string& filepath, const std::string& password) const {
    nlohmann::json json = toJson(false);

    if (!password.empty() && json.contains("accounts")) {
        const auto& accounts = getAccounts();
        auto& accountsJson = json["accounts"];

        if (accountsJson.is_array() && accountsJson.size() == accounts.size()) {
            for (size_t i = 0; i < accounts.size(); ++i) {
                const auto& account = accounts[i];
                if (account->getKeyPair() || !account->getEncryptedPrivateKey().empty()) {
                    try {
                        accountsJson[i]["key"] = account->exportNEP2(password);
                    } catch (const std::exception& e) {
                        LOG_DEBUG(std::string("Failed to export NEP-2: ") + e.what());
                        accountsJson[i]["key"] = nullptr;
                    }
                }
            }
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw WalletException("Failed to open file for writing: " + filepath);
    }
    
    file << json.dump(4);
    file.close();
}

nlohmann::json Nep6Wallet::toJson() const {
    return toJson(false);
}

nlohmann::json Nep6Wallet::toJson(bool includePlaintextWif) const {
    nlohmann::json json;
    json["name"] = getName();
    json["version"] = getVersion();
    
    nlohmann::json accountsJson = nlohmann::json::array();
    for (const auto& account : getAccounts()) {
        nlohmann::json accountJson;
        accountJson["address"] = account->getAddress();
        accountJson["label"] = account->getLabel();
        accountJson["isDefault"] = account->getIsDefault();
        accountJson["lock"] = account->isLocked();
        
        // Add key if account has private key and is not locked
        if (includePlaintextWif && !account->isLocked() && account->getKeyPair()) {
            try {
                accountJson["key"] = account->exportWIF();
            } catch (const std::exception& e) {
                LOG_DEBUG(std::string("Failed to export WIF: ") + e.what());
                accountJson["key"] = nullptr;
            }
        } else if (!account->getEncryptedPrivateKey().empty()) {
            accountJson["key"] = account->getEncryptedPrivateKey();
        } else {
            accountJson["key"] = nullptr;
        }
        
        // Add contract info if available
        if (account->getContract()) {
            accountJson["contract"] = {
                {"script", ""},  // TODO: Add contract script
                {"parameters", nlohmann::json::array()},  // TODO: Add parameters
                {"deployed", false}
            };
        }
        
        accountsJson.push_back(accountJson);
    }
    json["accounts"] = accountsJson;
    
    // Add extra data if present
    if (!extra_.empty()) {
        json["extra"] = extra_;
    }
    
    // Always add tokens array (empty if none present)
    json["tokens"] = tokens_;
    if (tokens_.empty()) {
        json["tokens"] = nlohmann::json::array();
    }
    
    // Add scrypt parameters for NEP-2 encryption
    json["scrypt"] = {
        {"n", 16384},
        {"r", 8},
        {"p", 8}
    };
    
    return json;
}

SharedPtr<Nep6Wallet> Nep6Wallet::fromJson(const nlohmann::json& json, const std::string& password) {
    // Use defaults for missing fields
    std::string name = json.value("name", "NeoCpp Wallet");
    std::string version = json.value("version", "1.0");
    
    auto wallet = std::make_shared<Nep6Wallet>(name, version);
    
    // Load extra data if present
    if (json.contains("extra")) {
        wallet->setExtra(json["extra"]);
    }
    
    // Load tokens if present
    if (json.contains("tokens")) {
        for (const auto& token : json["tokens"]) {
            wallet->addToken(token);
        }
    }
    
    // Load accounts
    if (json.contains("accounts")) {
        for (const auto& accJson : json["accounts"]) {
            if (!accJson.contains("address")) {
                continue;  // Skip invalid accounts
            }
            
            std::string address = accJson["address"].get<std::string>();
            std::string label = accJson.value("label", "");
            bool isDefault = accJson.value("isDefault", false);
            
            SharedPtr<Account> account;
            
            if (accJson.contains("key") && !accJson["key"].is_null()) {
                std::string key = accJson["key"].get<std::string>();
                if (key.length() == 58) {  // NEP-2 encrypted
                    try {
                        account = Account::fromNEP2(key, password, label);
                    } catch (const std::exception& e) {
                        LOG_DEBUG(std::string("Failed to decrypt NEP-2, creating watch-only account: ") + e.what());
                        account = Account::fromAddress(address, label);
                    }
                } else {
                    // WIF format
                    try {
                        account = Account::fromWIF(key, label);
                    } catch (const std::exception& e) {
                        LOG_DEBUG(std::string("Invalid WIF, creating watch-only account: ") + e.what());
                        account = Account::fromAddress(address, label);
                    }
                }
            } else {
                // Watch-only account
                account = Account::fromAddress(address, label);
            }
            
            if (account) {
                account->setIsDefault(isDefault);
                wallet->addAccount(account);
            }
        }
    }
    
    return wallet;
}

SharedPtr<Nep6Wallet> Nep6Wallet::load(const std::string& filepath, const std::string& password) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw WalletException("Failed to open file: " + filepath);
    }
    
    nlohmann::json json;
    try {
        file >> json;
    } catch (const std::exception& e) {
        throw WalletException("Failed to parse wallet JSON: " + std::string(e.what()));
    }
    
    file.close();
    
    return fromJson(json, password);
}

} // namespace neocpp
