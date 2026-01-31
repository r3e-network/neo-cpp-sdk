
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/types.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/script/script_builder.hpp"
#include <algorithm>
#include <cstring>

namespace neocpp {

// Static member initialization
const Hash160 Hash160::ZERO = Hash160();

Hash160::Hash160() {
    hash_.fill(0);
} // namespace neocpp
Hash160::Hash160(const Bytes& hash) {
    if (hash.size() != NeoConstants::HASH160_SIZE) {
        throw IllegalArgumentException("Hash must be " + std::to_string(NeoConstants::HASH160_SIZE) +
                                      " bytes long but was " + std::to_string(hash.size()) + " bytes.");
    }
    std::copy(hash.begin(), hash.end(), hash_.begin());
} // namespace neocpp
Hash160::Hash160(const std::array<uint8_t, NeoConstants::HASH160_SIZE>& hash) : hash_(hash) {
} // namespace neocpp
Hash160::Hash160(const std::string& hash) {
    Bytes bytes = ByteUtils::fromHex(hash);
    if (bytes.size() != NeoConstants::HASH160_SIZE) {
        throw IllegalArgumentException("Hash must be " + std::to_string(NeoConstants::HASH160_SIZE) +
                                      " bytes long but was " + std::to_string(bytes.size()) + " bytes.");
    }
    std::copy(bytes.begin(), bytes.end(), hash_.begin());
} // namespace neocpp
std::string Hash160::toString() const {
    return ByteUtils::toHex(Bytes(hash_.begin(), hash_.end()), false);
} // namespace neocpp
Bytes Hash160::toArray() const {
    return Bytes(hash_.begin(), hash_.end());
} // namespace neocpp
Bytes Hash160::toLittleEndianArray() const {
    Bytes result(hash_.begin(), hash_.end());
    std::reverse(result.begin(), result.end());
    return result;
} // namespace neocpp
std::string Hash160::toAddress() const {
    return AddressUtils::scriptHashToAddress(toArray());
} // namespace neocpp
Hash160 Hash160::fromAddress(const std::string& address) {
    return Hash160(AddressUtils::addressToScriptHash(address));
} // namespace neocpp
Hash160 Hash160::fromScript(const Bytes& script) {
    Bytes hash = HashUtils::sha256ThenRipemd160(script);
    std::reverse(hash.begin(), hash.end());
    return Hash160(hash);
} // namespace neocpp
Hash160 Hash160::fromScript(const std::string& script) {
    return fromScript(ByteUtils::fromHex(script));
} // namespace neocpp
Hash160 Hash160::fromPublicKey(const Bytes& encodedPublicKey) {
    return fromScript(ScriptBuilder::buildVerificationScript(encodedPublicKey));
} // namespace neocpp
Hash160 Hash160::fromPublicKeys(const std::vector<SharedPtr<ECPublicKey>>& pubKeys, int signingThreshold) {
    return fromScript(ScriptBuilder::buildVerificationScript(pubKeys, signingThreshold));
} // namespace neocpp
size_t Hash160::getSize() const {
    return NeoConstants::HASH160_SIZE;
} // namespace neocpp
void Hash160::serialize(BinaryWriter& writer) const {
    // Neo N3 serializes hashes in big-endian (same as hex string representation)
    writer.writeBytes(toArray());
} // namespace neocpp
Hash160 Hash160::deserialize(BinaryReader& reader) {
    Bytes bytes = reader.readBytes(NeoConstants::HASH160_SIZE);
    // Hash is stored in big-endian internally
    return Hash160(bytes);
} // namespace neocpp
bool Hash160::operator==(const Hash160& other) const {
    return hash_ == other.hash_;
} // namespace neocpp
bool Hash160::operator!=(const Hash160& other) const {
    return !(*this == other);
} // namespace neocpp
bool Hash160::operator<(const Hash160& other) const {
    return hash_ < other.hash_;
} // namespace neocpp
bool Hash160::operator<=(const Hash160& other) const {
    return hash_ <= other.hash_;
} // namespace neocpp
bool Hash160::operator>(const Hash160& other) const {
    return hash_ > other.hash_;
} // namespace neocpp
bool Hash160::operator>=(const Hash160& other) const {
    return hash_ >= other.hash_;
} // namespace neocpp
size_t Hash160::Hasher::operator()(const Hash160& hash) const {
    size_t result = 0;
    const auto& array = hash.hash_;
    for (size_t i = 0; i < std::min(sizeof(size_t), array.size()); ++i) {
        result |= static_cast<size_t>(array[i]) << (i * 8);
    }
    return result;
} // namespace neocpp
} // namespace neocpp
