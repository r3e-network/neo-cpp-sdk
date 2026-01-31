
#include "neocpp/wallet/wallet.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace neocpp {

Wallet::Wallet(const std::string& name, const std::string& version)
    : name_(name), version_(version) {
} // namespace neocpp
void Wallet::addAccount(const SharedPtr<Account>& account) {
    if (containsAccount(account->getAddress())) {
        throw WalletException("Account already exists in wallet");
    }

    accounts_.push_back(account);
    updateIndices();
} // namespace neocpp
bool Wallet::removeAccount(const std::string& address) {
    auto it = std::find_if(accounts_.begin(), accounts_.end(),
                          [&address](const SharedPtr<Account>& acc) {
                              return acc->getAddress() == address;
                          });

    if (it != accounts_.end()) {
        accounts_.erase(it);
        updateIndices();
        return true;
    }

    return false;
} // namespace neocpp
SharedPtr<Account> Wallet::getAccount(const std::string& address) const {
    auto it = accountsByAddress_.find(address);
    if (it != accountsByAddress_.end()) {
        return it->second;
    }
    return nullptr;
} // namespace neocpp
SharedPtr<Account> Wallet::getAccount(const Hash160& scriptHash) const {
    auto it = accountsByScriptHash_.find(scriptHash);
    if (it != accountsByScriptHash_.end()) {
        return it->second;
    }
    return nullptr;
} // namespace neocpp
SharedPtr<Account> Wallet::getDefaultAccount() const {
    for (const auto& account : accounts_) {
        if (account->getIsDefault()) {
            return account;
        }
    }

    // If no default set and accounts exist, return first
    if (!accounts_.empty()) {
        return accounts_[0];
    }

    return nullptr;
} // namespace neocpp
bool Wallet::setDefaultAccount(const std::string& address) {
    auto account = getAccount(address);
    if (!account) {
        return false;
    }

    // Clear previous default
    for (const auto& acc : accounts_) {
        acc->setIsDefault(false);
    }

    account->setIsDefault(true);
    return true;
} // namespace neocpp
bool Wallet::containsAccount(const std::string& address) const {
    return accountsByAddress_.find(address) != accountsByAddress_.end();
} // namespace neocpp
bool Wallet::containsAccount(const Hash160& scriptHash) const {
    return accountsByScriptHash_.find(scriptHash) != accountsByScriptHash_.end();
} // namespace neocpp
SharedPtr<Account> Wallet::createAccount(const std::string& label) {
    auto account = Account::create(label);
    addAccount(account);
    return account;
} // namespace neocpp
SharedPtr<Account> Wallet::importFromWIF(const std::string& wif, const std::string& label) {
    auto account = Account::fromWIF(wif, label);
    addAccount(account);
    return account;
} // namespace neocpp
SharedPtr<Account> Wallet::importFromNEP2(const std::string& nep2, const std::string& password, const std::string& label) {
    try {
        auto account = Account::fromNEP2(nep2, password, label);
        addAccount(account);
        return account;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("NEP2 import failed: ") + e.what());
        return nullptr;
    }
} // namespace neocpp
bool Wallet::signTransaction(const SharedPtr<Transaction>& transaction) {
    bool didSign = false;

    for (const auto& signer : transaction->getSigners()) {
        auto account = getAccount(signer->getAccount());
        if (account && !account->isLocked()) {
            // Sign the transaction hash
            Bytes txHash = transaction->getHashData();
            Bytes signature = account->sign(txHash);

            // Create witness
            auto witness = Witness::fromSignature(signature, account->getKeyPair()->getPublicKey()->getEncoded());
            transaction->addWitness(witness);
            didSign = true;
        }
    }

    return didSign;
} // namespace neocpp
void Wallet::clear() {
    accounts_.clear();
    accountsByAddress_.clear();
    accountsByScriptHash_.clear();
} // namespace neocpp
void Wallet::save(const std::string& filepath, const std::string& password) const {
    nlohmann::json json;
    json["name"] = name_;
    json["version"] = version_;
    json["scrypt"] = {
        {"n", 16384},
        {"r", 8},
        {"p", 8},
        {"dkLen", 64}
    };

    json["accounts"] = nlohmann::json::array();
    for (const auto& account : accounts_) {
        nlohmann::json accJson;
        accJson["address"] = account->getAddress();
        accJson["label"] = account->getLabel();
        accJson["isDefault"] = account->getIsDefault();
        accJson["lock"] = account->isLocked();

        if (!password.empty() && !account->isLocked()) {
            accJson["key"] = account->exportNEP2(password);
        } else if (!account->getEncryptedPrivateKey().empty()) {
            accJson["key"] = account->getEncryptedPrivateKey();
        }

        if (account->getContract()) {
            accJson["contract"] = {
                {"script", ByteUtils::toHex(account->getVerificationScript(), false)},
                {"deployed", false}
            };
        }

        json["accounts"].push_back(accJson);
    }

    json["extra"] = nullptr;

    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw WalletException("Failed to open file for writing");
    }

    file << json.dump(4);
    file.close();
} // namespace neocpp
SharedPtr<Wallet> Wallet::load(const std::string& filepath, const std::string& password) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw WalletException("Failed to open wallet file");
    }

    nlohmann::json json;
    file >> json;
    file.close();

    auto wallet = std::make_shared<Wallet>(
        json.value("name", "Wallet"),
        json.value("version", "1.0")
    );

    for (const auto& accJson : json["accounts"]) {
        std::string address = accJson["address"].get<std::string>();
        std::string label = accJson.value("label", "");
        bool isDefault = accJson.value("isDefault", false);

        SharedPtr<Account> account;

        if (accJson.contains("key") && !accJson["key"].is_null()) {
            std::string key = accJson["key"].get<std::string>();
            try {
                if (key.length() == 58) { // NEP-2 encrypted
                    account = Account::fromNEP2(key, password, label);
                } else {
                    account = Account::fromWIF(key, label);
                }
            } catch (const std::exception& e) {
                LOG_DEBUG(std::string("Failed to decrypt/import key, creating watch-only account: ") + e.what());
                account = Account::fromAddress(address, label);
            }
        } else {
            // Watch-only account - create from address
            account = Account::fromAddress(address, label);
        }

        account->setIsDefault(isDefault);
        wallet->addAccount(account);
    }

    return wallet;
} // namespace neocpp
void Wallet::updateIndices() {
    accountsByAddress_.clear();
    accountsByScriptHash_.clear();

    for (const auto& account : accounts_) {
        accountsByAddress_[account->getAddress()] = account;
        accountsByScriptHash_[account->getScriptHash()] = account;
    }
} // namespace neocpp
} // namespace neocpp
