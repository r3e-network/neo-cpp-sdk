
#include "neocpp/types/hash256.hpp"
#include "neocpp/types/types.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"
#include <algorithm>
#include <cstring>

namespace neocpp {

// Static member initialization
const Hash256 Hash256::ZERO = Hash256();

Hash256::Hash256() {
    hash_.fill(0);
} // namespace neocpp
Hash256::Hash256(const Bytes& hash) {
    if (hash.size() != NeoConstants::HASH256_SIZE) {
        throw IllegalArgumentException("Hash must be " + std::to_string(NeoConstants::HASH256_SIZE) +
                                      " bytes long but was " + std::to_string(hash.size()) + " bytes.");
    }
    std::copy(hash.begin(), hash.end(), hash_.begin());
} // namespace neocpp
Hash256::Hash256(const std::array<uint8_t, NeoConstants::HASH256_SIZE>& hash) : hash_(hash) {
} // namespace neocpp
Hash256::Hash256(const std::string& hash) {
    Bytes bytes = Hex::decode(hash);
    if (bytes.size() != NeoConstants::HASH256_SIZE) {
        throw IllegalArgumentException("Hash must be " + std::to_string(NeoConstants::HASH256_SIZE) +
                                      " bytes long but was " + std::to_string(bytes.size()) + " bytes.");
    }
    std::copy(bytes.begin(), bytes.end(), hash_.begin());
} // namespace neocpp
std::string Hash256::toString() const {
    return Hex::encode(Bytes(hash_.begin(), hash_.end()));
} // namespace neocpp
Bytes Hash256::toArray() const {
    return Bytes(hash_.begin(), hash_.end());
} // namespace neocpp
Bytes Hash256::toLittleEndianArray() const {
    Bytes result(hash_.begin(), hash_.end());
    std::reverse(result.begin(), result.end());
    return result;
} // namespace neocpp
size_t Hash256::getSize() const {
    return NeoConstants::HASH256_SIZE;
} // namespace neocpp
void Hash256::serialize(BinaryWriter& writer) const {
    // Neo N3 serializes hashes in big-endian (same as hex string representation)
    writer.writeBytes(toArray());
} // namespace neocpp
Hash256 Hash256::deserialize(BinaryReader& reader) {
    Bytes bytes = reader.readBytes(NeoConstants::HASH256_SIZE);
    // Hash is stored in big-endian internally
    return Hash256(bytes);
} // namespace neocpp
bool Hash256::operator==(const Hash256& other) const {
    return hash_ == other.hash_;
} // namespace neocpp
bool Hash256::operator!=(const Hash256& other) const {
    return !(*this == other);
} // namespace neocpp
bool Hash256::operator<(const Hash256& other) const {
    return hash_ < other.hash_;
} // namespace neocpp
bool Hash256::operator<=(const Hash256& other) const {
    return hash_ <= other.hash_;
} // namespace neocpp
bool Hash256::operator>(const Hash256& other) const {
    return hash_ > other.hash_;
} // namespace neocpp
bool Hash256::operator>=(const Hash256& other) const {
    return hash_ >= other.hash_;
} // namespace neocpp
size_t Hash256::Hasher::operator()(const Hash256& hash) const {
    size_t result = 0;
    const auto& array = hash.hash_;
    for (size_t i = 0; i < std::min(sizeof(size_t), array.size()); ++i) {
        result |= static_cast<size_t>(array[i]) << (i * 8);
    }
    return result;
} // namespace neocpp
Hash256 Hash256::fromHexString(const std::string& hex) {
    return Hash256(hex);
} // namespace neocpp
} // namespace neocpp
