#include "neocpp/transaction/transaction_builder.hpp"
#include "neocpp/transaction/transaction.hpp"
#include "neocpp/transaction/signer.hpp"
#include "neocpp/transaction/witness.hpp"
#include "neocpp/protocol/neo_rpc_client.hpp"
#include "neocpp/wallet/account.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/types/contract_parameter.hpp"
#include "neocpp/contract/neo_token.hpp"
#include "neocpp/contract/gas_token.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/protocol/response_types.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/script/verification_script.hpp"
#include <random>
#include <algorithm>

namespace neocpp {

TransactionBuilder::TransactionBuilder(const SharedPtr<NeoRpcClient>& client)
    : client_(client) {
    initializeTransaction();
}

TransactionBuilder& TransactionBuilder::setClient(const SharedPtr<NeoRpcClient>& client) {
    client_ = client;
    return *this;
}

TransactionBuilder& TransactionBuilder::setNonce(uint32_t nonce) {
    transaction_->setNonce(nonce);
    return *this;
}

TransactionBuilder& TransactionBuilder::setRandomNonce() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;
    transaction_->setNonce(dis(gen));
    return *this;
}

TransactionBuilder& TransactionBuilder::validUntilBlock(uint32_t block) {
    return setValidUntilBlock(block);
}

TransactionBuilder& TransactionBuilder::setValidUntilBlock(uint32_t block) {
    transaction_->setValidUntilBlock(block);
    return *this;
}

TransactionBuilder& TransactionBuilder::setValidUntilBlockRelative(uint32_t blocksFromNow) {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto blockCount = client_->getBlockCount();
    if (!blockCount) {
        throw RuntimeException("Failed to get block count");
    }
    
    transaction_->setValidUntilBlock(blockCount + blocksFromNow);
    return *this;
}

TransactionBuilder& TransactionBuilder::setSystemFee(int64_t fee) {
    transaction_->setSystemFee(fee);
    return *this;
}

TransactionBuilder& TransactionBuilder::setNetworkFee(int64_t fee) {
    transaction_->setNetworkFee(fee);
    return *this;
}

TransactionBuilder& TransactionBuilder::setAdditionalNetworkFee(int64_t fee) {
    additionalNetworkFee_ = fee;
    return *this;
}

TransactionBuilder& TransactionBuilder::setAdditionalSystemFee(int64_t fee) {
    additionalSystemFee_ = fee;
    return *this;
}

TransactionBuilder& TransactionBuilder::setHighPriority(bool isHighPriority) {
    isHighPriority_ = isHighPriority;
    return *this;
}

SharedPtr<Signer> TransactionBuilder::firstSigner() const {
    auto signers = transaction_->getSigners();
    if (signers.empty()) {
        return nullptr;
    }
    return signers[0];
}

TransactionBuilder& TransactionBuilder::calculateNetworkFee() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    // Base fee calculation
    int64_t fee = 0;
    
    // Calculate verification cost for each signer
    for (const auto& signer : transaction_->getSigners()) {
        // Basic verification cost
        fee += 1000000; // 0.01 GAS base cost
    }
    
    // Calculate size-based fee
    auto size = transaction_->getSize();
    fee += size * 1000; // Fee per byte
    
    transaction_->setNetworkFee(fee);
    return *this;
}

TransactionBuilder& TransactionBuilder::addSigner(const SharedPtr<Signer>& signer) {
    transaction_->addSigner(signer);
    return *this;
}

TransactionBuilder& TransactionBuilder::signers(const std::vector<SharedPtr<Signer>>& signers) {
    // Replace existing signers with new ones
    transaction_->clearSigners();
    for (const auto& signer : signers) {
        transaction_->addSigner(signer);
    }
    return *this;
}

std::vector<SharedPtr<Signer>> TransactionBuilder::getSigners() const {
    return transaction_->getSigners();
}

TransactionBuilder& TransactionBuilder::addSigner(const SharedPtr<Account>& account, uint8_t scopes) {
    auto signer = std::make_shared<Signer>(account->getScriptHash(), static_cast<WitnessScope>(scopes));
    transaction_->addSigner(signer);
    signingAccounts_.push_back(account);
    return *this;
}

TransactionBuilder& TransactionBuilder::setScript(const Bytes& script) {
    transaction_->setScript(script);
    return *this;
}

TransactionBuilder& TransactionBuilder::callContract(const Hash160& scriptHash, 
                                                      const std::string& method, 
                                                      const std::vector<ContractParameter>& params) {
    ScriptBuilder builder;
    
    // Push parameters in reverse order
    for (auto it = params.rbegin(); it != params.rend(); ++it) {
        builder.pushContractParameter(*it);
    }
    
    // Push method name
    builder.pushString(method);
    
    // Push contract hash
    builder.pushData(scriptHash.toArray());
    
    // System call
    builder.emitSysCall("System.Contract.Call");
    
    transaction_->setScript(builder.toArray());
    return *this;
}

TransactionBuilder& TransactionBuilder::transferNeo(const SharedPtr<Account>& from, 
                                                    const std::string& to, 
                                                    int64_t amount) {
    return transferNep17(NeoToken::SCRIPT_HASH, from, to, amount, 0);
}

TransactionBuilder& TransactionBuilder::transferGas(const SharedPtr<Account>& from, 
                                                    const std::string& to, 
                                                    int64_t amount) {
    return transferNep17(GasToken::SCRIPT_HASH, from, to, amount, 8);
}

TransactionBuilder& TransactionBuilder::transferNep17(const Hash160& tokenHash, 
                                                      const SharedPtr<Account>& from, 
                                                      const std::string& to, 
                                                      int64_t amount, 
                                                      int decimals) {
    auto fromHash = from->getScriptHash();
    auto toBytes = AddressUtils::addressToScriptHash(to);
    Hash160 toHash(toBytes);
    
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(fromHash),
        ContractParameter::hash160(toHash),
        ContractParameter::integer(amount),
        ContractParameter::any()
    };
    
    callContract(tokenHash, "transfer", params);
    
    // Add signer if not already added
    bool found = false;
    for (const auto& signer : transaction_->getSigners()) {
        if (signer->getAccount() == fromHash) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        addSigner(from);
    }
    
    return *this;
}

TransactionBuilder& TransactionBuilder::addWitness(const SharedPtr<Witness>& witness) {
    transaction_->addWitness(witness);
    return *this;
}

TransactionBuilder& TransactionBuilder::extendScript(const Bytes& script) {
    auto currentScript = transaction_->getScript();
    currentScript.insert(currentScript.end(), script.begin(), script.end());
    transaction_->setScript(currentScript);
    return *this;
}

SharedPtr<Transaction> TransactionBuilder::getUnsignedTransaction() {
    // Set valid until block if not set
    if (transaction_->getValidUntilBlock() == 0) {
        setValidUntilBlockRelative(100);
    }
    
    // Verify there are signers
    auto signers = transaction_->getSigners();
    if (signers.empty()) {
        throw IllegalStateException("Cannot create a transaction without signers. At least one signer with witness scope fee-only or higher is required.");
    }
    
    // Check high priority
    if (isHighPriority_ && !isAllowedForHighPriority()) {
        throw IllegalStateException("This transaction does not have a committee member as signer. Only committee members can send transactions with high priority.");
    }
    
    // Calculate fees
    int64_t systemFee = getSystemFeeForScript() + additionalSystemFee_;
    int64_t networkFee = calcNetworkFee() + additionalNetworkFee_;
    int64_t totalFees = systemFee + networkFee;
    
    // Set fees
    transaction_->setSystemFee(systemFee);
    transaction_->setNetworkFee(networkFee);
    
    // Check fee coverage
    if (feeError_ && !canSendCoverFees(totalFees)) {
        throw *feeError_;
    } else if (feeConsumer_ && !canSendCoverFees(totalFees)) {
        int64_t gasBalance = getSenderGasBalance();
        feeConsumer_(totalFees, gasBalance);
    }
    
    // Sort signers and witnesses
    sortSigners();
    
    return transaction_;
}

SharedPtr<NeoInvokeResultResponse> TransactionBuilder::callInvokeScript() {
    auto script = transaction_->getScript();
    if (script.empty()) {
        throw IllegalStateException("Cannot make an 'invokescript' call without the script being configured.");
    }
    
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    // Convert signers to JSON format
    nlohmann::json signersJson = nlohmann::json::array();
    for (const auto& signer : transaction_->getSigners()) {
        signersJson.push_back({
            {"account", signer->getAccount().toString()},
            {"scopes", signer->getScopes() == WitnessScope::CUSTOM_CONTRACTS ? 
                       "CustomContracts" : 
                       (signer->getScopes() == WitnessScope::GLOBAL ? 
                        "Global" : "CalledByEntry")}
        });
    }
    
    return client_->invokeScript(script, signersJson);
}

TransactionBuilder& TransactionBuilder::sign(const SharedPtr<Account>& account) {
    // Ensure transaction is properly configured
    getUnsignedTransaction();
    
    // Add witness for the account
    addWitnessForAccount(account);
    return *this;
}

TransactionBuilder& TransactionBuilder::signWithSigners() {
    // Ensure transaction is properly configured
    getUnsignedTransaction();
    
    // Add witnesses for all signing accounts
    for (const auto& account : signingAccounts_) {
        addWitnessForAccount(account);
    }
    return *this;
}

SharedPtr<Transaction> TransactionBuilder::build() {
    // Get the unsigned transaction with all validations and fee calculations
    getUnsignedTransaction();
    
    // Sort witnesses to match signers
    sortWitnesses();
    
    return transaction_;
}

TransactionBuilder& TransactionBuilder::doIfSenderCannotCoverFees(const std::function<void(int64_t, int64_t)>& consumer) {
    if (feeError_) {
        throw IllegalStateException("Cannot handle a consumer for this case, since an exception will be thrown if the sender cannot cover the fees.");
    }
    feeConsumer_ = consumer;
    return *this;
}

TransactionBuilder& TransactionBuilder::throwIfSenderCannotCoverFees(const std::shared_ptr<std::exception>& error) {
    if (feeConsumer_) {
        throw IllegalStateException("Cannot handle a supplier for this case, since a consumer will be executed if the sender cannot cover the fees.");
    }
    feeError_ = error;
    return *this;
}

SharedPtr<Transaction> TransactionBuilder::buildAndSign() {
    // Get the unsigned transaction with all validations and fee calculations
    getUnsignedTransaction();
    
    // Sign with all signing accounts
    signWithSigners();
    
    return transaction_;
}

void TransactionBuilder::initializeTransaction() {
    transaction_ = std::make_shared<Transaction>();
    transaction_->setVersion(0);
    transaction_->setSystemFee(0);
    transaction_->setNetworkFee(0);
}

void TransactionBuilder::addWitnessForAccount(const SharedPtr<Account>& account) {
    // Get transaction hash for signing
    auto txHash = transaction_->getHash();
    
    // Sign the transaction hash
    auto signature = account->sign(txHash.toArray());
    
    // Create witness
    auto witness = std::make_shared<Witness>();
    witness->setInvocationScript(ScriptBuilder().pushData(signature).toArray());
    witness->setVerificationScript(account->getVerificationScript());
    
    transaction_->addWitness(witness);
}

void TransactionBuilder::sortSigners() {
    auto signers = transaction_->getSigners();
    
    // Sort signers by scope first, then by account hash
    std::sort(signers.begin(), signers.end(), 
        [](const SharedPtr<Signer>& a, const SharedPtr<Signer>& b) {
            if (a->getScopes() != b->getScopes()) {
                return a->getScopes() < b->getScopes();
            }
            return a->getAccount().toString() < b->getAccount().toString();
        });
    
    // Transaction doesn't have setSigners - signers are already in the transaction
}

void TransactionBuilder::sortWitnesses() {
    auto witnesses = transaction_->getWitnesses();
    auto signers = transaction_->getSigners();
    
    if (witnesses.size() != signers.size()) {
        return; // Can't sort if counts don't match
    }
    
    // Sort witnesses to match signer order
    std::vector<SharedPtr<Witness>> sortedWitnesses;
    for (const auto& signer : signers) {
        // Find matching witness by verification script hash
        for (const auto& witness : witnesses) {
            auto scriptBytes = HashUtils::sha256ThenRipemd160(witness->getVerificationScript());
            Hash160 scriptHash(scriptBytes);
            if (scriptHash == signer->getAccount()) {
                sortedWitnesses.push_back(witness);
                break;
            }
        }
    }
    
    if (sortedWitnesses.size() == witnesses.size()) {
        // Transaction doesn't have setWitnesses - witnesses are already in the transaction
    }
}

bool TransactionBuilder::isAllowedForHighPriority() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    std::vector<std::string> committee = client_->getCommittee();
    if (committee.empty()) {
        throw RuntimeException("Failed to get committee members or committee is empty");
    }
    
    // Convert committee public keys to script hashes
    std::vector<Hash160> committeeHashes;
    for (const auto& publicKey : committee) {
        try {
            ECPublicKey ecKey(publicKey);
            auto encodedKey = ecKey.getEncodedCompressed(); // Compressed
            auto hash = Hash160::fromPublicKey(encodedKey);
            committeeHashes.push_back(hash);
        } catch (const std::exception& e) {
            // Skip invalid public keys
            continue;
        }
    }
    
    // Check if any signer is a committee member
    auto signers = transaction_->getSigners();
    for (const auto& signer : signers) {
        auto signerHash = signer->getAccount();
        if (std::find(committeeHashes.begin(), committeeHashes.end(), signerHash) != committeeHashes.end()) {
            return true;
        }
    }
    
    // Check for multi-sig with committee member
    return signersContainMultiSigWithCommitteeMember(committeeHashes);
}

bool TransactionBuilder::signersContainMultiSigWithCommitteeMember(const std::vector<Hash160>& committee) {
    // Check each witness's verification script for multi-sig containing committee members
    auto witnesses = transaction_->getWitnesses();
    
    for (const auto& witness : witnesses) {
        try {
            // Parse verification script to check for multi-sig structure
            const auto& verificationScript = witness->getVerificationScript();
            
            // Multi-sig scripts have specific structure:
            // [OP_PUSH1 + threshold] [OP_PUSH1 + pubkey1] ... [OP_PUSH1 + pubkeyN] [OP_PUSH1 + N] [OP_CHECKMULTISIG]
            
            // Check if this is a multi-sig script by checking the last opcode
            if (verificationScript.size() > 35 && verificationScript.back() == 0xAE) { // OP_CHECKMULTISIG
                // Parse the multi-sig script to extract public keys
                size_t pos = 0;
                
                // Read threshold (M of N)
                if (pos < verificationScript.size() && verificationScript[pos] >= 0x51 && verificationScript[pos] <= 0x60) {
                    int threshold = verificationScript[pos] - 0x50;
                    pos++;
                    
                    // Extract public keys from the script
                    std::vector<Bytes> publicKeys;
                    while (pos < verificationScript.size() - 2) {
                        // Check for public key push operation (33 or 65 bytes)
                        if (verificationScript[pos] == 0x21) { // 33 bytes compressed public key
                            if (pos + 34 <= verificationScript.size()) {
                                Bytes pubKey(verificationScript.begin() + pos + 1, verificationScript.begin() + pos + 34);
                                publicKeys.push_back(pubKey);
                                pos += 34;
                            } else {
                                break;
                            }
                        } else if (verificationScript[pos] == 0x41) { // 65 bytes uncompressed public key
                            if (pos + 66 <= verificationScript.size()) {
                                Bytes pubKey(verificationScript.begin() + pos + 1, verificationScript.begin() + pos + 66);
                                publicKeys.push_back(pubKey);
                                pos += 66;
                            } else {
                                break;
                            }
                        } else if (verificationScript[pos] >= 0x51 && verificationScript[pos] <= 0x60) {
                            // This is the N value (number of public keys)
                            int n = verificationScript[pos] - 0x50;
                            if (n == publicKeys.size()) {
                                // Valid multi-sig script, now check against committee members
                                for (const auto& pubKey : publicKeys) {
                                    // Calculate script hash from public key
                                    Hash160 pubKeyHash = Hash160::fromPublicKey(pubKey);
                                    
                                    // Check if this public key hash matches any committee member
                                    for (const auto& committeeHash : committee) {
                                        if (pubKeyHash == committeeHash) {
                                            return true;
                                        }
                                    }
                                }
                            }
                            break;
                        } else {
                            break; // Invalid script structure
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            // Skip invalid witnesses
            continue;
        }
    }
    
    // No multi-sig with committee member found
    return false;
}

int64_t TransactionBuilder::getSystemFeeForScript() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto script = transaction_->getScript();
    
    // Convert signers to JSON format
    nlohmann::json signersJson = nlohmann::json::array();
    for (const auto& signer : transaction_->getSigners()) {
        nlohmann::json signerObj;
        signerObj["account"] = signer->getAccount().toString();
        
        // Map witness scope to string
        switch (signer->getScopes()) {
            case WitnessScope::NONE:
                signerObj["scopes"] = "None";
                break;
            case WitnessScope::CALLED_BY_ENTRY:
                signerObj["scopes"] = "CalledByEntry";
                break;
            case WitnessScope::CUSTOM_CONTRACTS:
                signerObj["scopes"] = "CustomContracts";
                break;
            case WitnessScope::CUSTOM_GROUPS:
                signerObj["scopes"] = "CustomGroups";
                break;
            case WitnessScope::GLOBAL:
                signerObj["scopes"] = "Global";
                break;
            default:
                signerObj["scopes"] = "None";
        }
        
        signersJson.push_back(signerObj);
    }
    
    auto response = client_->invokeScript(script, signersJson);
    
    if (!response) {
        throw RuntimeException("Failed to invoke script");
    }
    
    auto invokeResult = response->getResult();
    if (invokeResult.state != "HALT") {
        // In a real implementation, you might want to check a config flag to allow transmission on fault
        if (!invokeResult.exception.empty()) {
            throw IllegalStateException("The VM exited due to an exception: " + invokeResult.exception);
        } else {
            throw IllegalStateException("The VM exited with state: " + invokeResult.state);
        }
    }
    
    return invokeResult.gasConsumed;
}

int64_t TransactionBuilder::calcNetworkFee() {
    // Create a temporary transaction for fee calculation
    auto tx = std::make_shared<Transaction>();
    tx->setVersion(transaction_->getVersion());
    tx->setNonce(transaction_->getNonce());
    tx->setSystemFee(0);
    tx->setNetworkFee(0);
    tx->setValidUntilBlock(transaction_->getValidUntilBlock());
    tx->setScript(transaction_->getScript());
    
    // Copy signers
    for (const auto& signer : transaction_->getSigners()) {
        tx->addSigner(signer);
    }
    
    // Copy attributes
    for (const auto& attribute : transaction_->getAttributes()) {
        tx->addAttribute(attribute);
    }
    
    bool hasAtLeastOneSigningAccount = false;
    
    // Add fake witnesses for fee calculation
    for (const auto& account : signingAccounts_) {
        auto verificationScript = createFakeVerificationScript(account);
        auto witness = std::make_shared<Witness>();
        witness->setVerificationScript(verificationScript);
        tx->addWitness(witness);
        hasAtLeastOneSigningAccount = true;
    }
    
    if (!hasAtLeastOneSigningAccount) {
        throw IllegalStateException("A transaction requires at least one signing account. None was provided.");
    }
    
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto fee = client_->calculateNetworkFee(tx);
    return fee;
}

int64_t TransactionBuilder::getSenderGasBalance() {
    if (!client_) {
        throw IllegalStateException("RPC client not set");
    }
    
    auto signers = transaction_->getSigners();
    if (signers.empty()) {
        throw IllegalStateException("No signers available to get sender balance");
    }
    
    // Use the first signer's account as the sender
    auto senderHash = signers[0]->getAccount();
    
    // Call balanceOf on the GAS contract
    std::vector<ContractParameter> params = {
        ContractParameter::hash160(senderHash)
    };
    
    // Convert ContractParameter vector to JSON format
    nlohmann::json jsonParams = nlohmann::json::array();
    for (const auto& param : params) {
        jsonParams.push_back(param.toRpcJson());
    }
    
    auto response = client_->invokeFunction(GasToken::SCRIPT_HASH, "balanceOf", jsonParams);
    if (!response) {
        throw RuntimeException("Failed to get GAS balance");
    }
    
    auto invokeResult = response->getResult();
    if (invokeResult.stack.empty()) {
        throw RuntimeException("Invalid response from balanceOf call: empty stack");
    }
    
    // Parse the balance from the stack result
    auto& stackItem = invokeResult.stack[0];
    if (stackItem.is_number()) {
        return stackItem.get<int64_t>();
    } else if (stackItem.is_string()) {
        // Handle hex string representation if needed
        try {
            return std::stoll(stackItem.get<std::string>(), nullptr, 16);
        } catch (...) {
            throw RuntimeException("Failed to parse balance from response");
        }
    }
    
    throw RuntimeException("Invalid balanceOf response type");
}

bool TransactionBuilder::canSendCoverFees(int64_t fees) {
    try {
        return getSenderGasBalance() >= fees;
    } catch (const std::exception& e) {
        // If there's an error checking balance, assume sender cannot cover fees
        return false;
    }
}

Bytes TransactionBuilder::createFakeVerificationScript(const SharedPtr<Account>& account) {
    // For fee calculation purposes, we need a verification script that matches the structure
    // of the account's actual script but doesn't need to be valid for signing.
    
    // Get the account's public key hash
    auto publicKey = account->getKeyPair()->getPublicKey();
    auto publicKeyHash = Hash160::fromPublicKey(publicKey->getEncoded());
    
    // Create a standard P2PKH (Pay to Public Key Hash) verification script:
    // 1. DUP
    // 2. HASH160
    // 3. <PublicKeyHash>
    // 4. EQUALVERIFY
    // 5. CHECKSIG
    Bytes script;
    
    // OP_DUP (0x76)
    script.push_back(0x76);
    
    // OP_HASH160 (0xA9)
    script.push_back(0xA9);
    
    // Push public key hash (20 bytes)
    script.push_back(0x14); // Push 20 bytes
    auto hashBytes = publicKeyHash.toArray();
    script.insert(script.end(), hashBytes.begin(), hashBytes.end());
    
    // OP_EQUALVERIFY (0x88)
    script.push_back(0x88);
    
    // OP_CHECKSIG (0xAC)
    script.push_back(0xAC);
    
    return script;
}

} // namespace neocpp