#include "neocpp/transaction/neo_transaction.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/hex.hpp"

namespace neocpp {

NeoTransaction::NeoTransaction(
    std::shared_ptr<NeoCpp> neoCpp,
    uint8_t version,
    uint32_t nonce,
    uint32_t validUntilBlock,
    std::vector<Signer> signers,
    int64_t systemFee,
    int64_t networkFee,
    std::vector<TransactionAttribute> attributes,
    Bytes script,
    std::vector<Witness> witnesses,
    std::optional<uint32_t> blockCountWhenSent
) : neoCpp_(neoCpp),
    version_(version),
    nonce_(nonce),
    validUntilBlock_(validUntilBlock),
    signers_(std::move(signers)),
    systemFee_(systemFee),
    networkFee_(networkFee),
    attributes_(std::move(attributes)),
    script_(std::move(script)),
    witnesses_(std::move(witnesses)),
    blockCountWhenSent_(blockCountWhenSent) {
}

Bytes NeoTransaction::getHash() const {
    // TODO: Implement proper transaction serialization and hashing
    // This is a placeholder implementation
    Bytes serialized;
    
    // Add version
    serialized.push_back(version_);
    
    // Add nonce (4 bytes)
    serialized.push_back((nonce_ >> 0) & 0xFF);
    serialized.push_back((nonce_ >> 8) & 0xFF);
    serialized.push_back((nonce_ >> 16) & 0xFF);
    serialized.push_back((nonce_ >> 24) & 0xFF);
    
    // Add validUntilBlock (4 bytes)
    serialized.push_back((validUntilBlock_ >> 0) & 0xFF);
    serialized.push_back((validUntilBlock_ >> 8) & 0xFF);
    serialized.push_back((validUntilBlock_ >> 16) & 0xFF);
    serialized.push_back((validUntilBlock_ >> 24) & 0xFF);
    
    // Add script
    serialized.insert(serialized.end(), script_.begin(), script_.end());
    
    // Hash the serialized transaction
    return HashUtils::doubleSha256(serialized);
}

std::string NeoTransaction::getHashHex() const {
    return Hex::encode(getHash());
}

void NeoTransaction::addWitness(const Witness& witness) {
    witnesses_.push_back(witness);
}

void NeoTransaction::setWitnesses(const std::vector<Witness>& witnesses) {
    witnesses_ = witnesses;
}

std::shared_ptr<NeoCpp> NeoTransaction::getNeoCpp() const {
    return neoCpp_;
}

uint8_t NeoTransaction::getVersion() const {
    return version_;
}

uint32_t NeoTransaction::getNonce() const {
    return nonce_;
}

uint32_t NeoTransaction::getValidUntilBlock() const {
    return validUntilBlock_;
}

const std::vector<Signer>& NeoTransaction::getSigners() const {
    return signers_;
}

int64_t NeoTransaction::getSystemFee() const {
    return systemFee_;
}

int64_t NeoTransaction::getNetworkFee() const {
    return networkFee_;
}

const std::vector<TransactionAttribute>& NeoTransaction::getAttributes() const {
    return attributes_;
}

const Bytes& NeoTransaction::getScript() const {
    return script_;
}

const std::vector<Witness>& NeoTransaction::getWitnesses() const {
    return witnesses_;
}

std::optional<uint32_t> NeoTransaction::getBlockCountWhenSent() const {
    return blockCountWhenSent_;
}

void NeoTransaction::setBlockCountWhenSent(uint32_t blockCount) {
    blockCountWhenSent_ = blockCount;
}

} // namespace neocpp