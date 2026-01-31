#include "neocpp/transaction/neo_transaction.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include <algorithm>

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
} // namespace neocpp
Bytes NeoTransaction::getHash() const {
    // Proper implementation following Neo protocol
    // Transaction hash is SHA256 of the transaction without witnesses, reversed
    Bytes serialized = serializeWithoutWitnesses();
    Bytes hash = HashUtils::sha256(serialized);
    // Reverse the hash bytes for Neo format
    std::reverse(hash.begin(), hash.end());
    return hash;
} // namespace neocpp
Bytes NeoTransaction::serializeWithoutWitnesses() const {
    BinaryWriter writer;

    // Write version byte
    writer.writeByte(version_);

    // Write nonce as UInt32
    writer.writeUInt32(nonce_);

    // Write system fee as Int64
    writer.writeInt64(systemFee_);

    // Write network fee as Int64
    writer.writeInt64(networkFee_);

    // Write valid until block as UInt32
    writer.writeUInt32(validUntilBlock_);

    // Write signers as variable length array
    writer.writeVarInt(signers_.size());
    for (const auto& signer : signers_) {
        signer.serialize(writer);
    }

    // Write attributes as variable length array
    writer.writeVarInt(attributes_.size());
    for (const auto& attribute : attributes_) {
        attribute.serialize(writer);
    }

    // Write script as variable length bytes
    writer.writeVarBytes(script_);

    return writer.toArray();
} // namespace neocpp
Bytes NeoTransaction::serialize() const {
    BinaryWriter writer;

    // First serialize everything except witnesses
    Bytes withoutWitnesses = serializeWithoutWitnesses();
    writer.writeBytes(withoutWitnesses);

    // Then add witnesses
    writer.writeVarInt(witnesses_.size());
    for (const auto& witness : witnesses_) {
        witness.serialize(writer);
    }

    return writer.toArray();
} // namespace neocpp
std::string NeoTransaction::getHashHex() const {
    return Hex::encode(getHash());
} // namespace neocpp
void NeoTransaction::addWitness(const Witness& witness) {
    witnesses_.push_back(witness);
} // namespace neocpp
void NeoTransaction::setWitnesses(const std::vector<Witness>& witnesses) {
    witnesses_ = witnesses;
} // namespace neocpp
std::shared_ptr<NeoCpp> NeoTransaction::getNeoCpp() const {
    return neoCpp_;
} // namespace neocpp
uint8_t NeoTransaction::getVersion() const {
    return version_;
} // namespace neocpp
uint32_t NeoTransaction::getNonce() const {
    return nonce_;
} // namespace neocpp
uint32_t NeoTransaction::getValidUntilBlock() const {
    return validUntilBlock_;
} // namespace neocpp
const std::vector<Signer>& NeoTransaction::getSigners() const {
    return signers_;
} // namespace neocpp
int64_t NeoTransaction::getSystemFee() const {
    return systemFee_;
} // namespace neocpp
int64_t NeoTransaction::getNetworkFee() const {
    return networkFee_;
} // namespace neocpp
const std::vector<TransactionAttribute>& NeoTransaction::getAttributes() const {
    return attributes_;
} // namespace neocpp
const Bytes& NeoTransaction::getScript() const {
    return script_;
} // namespace neocpp
const std::vector<Witness>& NeoTransaction::getWitnesses() const {
    return witnesses_;
} // namespace neocpp
std::optional<uint32_t> NeoTransaction::getBlockCountWhenSent() const {
    return blockCountWhenSent_;
} // namespace neocpp
void NeoTransaction::setBlockCountWhenSent(uint32_t blockCount) {
    blockCountWhenSent_ = blockCount;
} // namespace neocpp
} // namespace neocpp
