
#include "neocpp/transaction/transaction_attribute.hpp"
#include "neocpp/serialization/binary_writer.hpp"
#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

void TransactionAttribute::serialize(BinaryWriter& writer) const {
    writer.writeUInt8(static_cast<uint8_t>(getType()));
    serializeWithoutType(writer);
} // namespace neocpp
SharedPtr<TransactionAttribute> TransactionAttribute::highPriority() {
    return std::make_shared<HighPriorityAttribute>();
} // namespace neocpp
SharedPtr<TransactionAttribute> TransactionAttribute::deserialize(BinaryReader& reader) {
    auto type = static_cast<TransactionAttributeType>(reader.readUInt8());

    SharedPtr<TransactionAttribute> attribute;

    switch (type) {
        case TransactionAttributeType::HIGH_PRIORITY:
            attribute = std::make_shared<HighPriorityAttribute>();
            break;
        case TransactionAttributeType::ORACLE_RESPONSE: {
            uint64_t id = reader.readUInt64();
            uint8_t code = reader.readUInt8();
            Bytes result = reader.readVarBytes();
            attribute = std::make_shared<OracleResponseAttribute>(id, code, result);
            break;
        }
        case TransactionAttributeType::NOT_VALID_BEFORE: {
            uint32_t height = reader.readUInt32();
            attribute = std::make_shared<NotValidBeforeAttribute>(height);
            break;
        }
        case TransactionAttributeType::CONFLICTS: {
            Hash256 hash = Hash256::deserialize(reader);
            attribute = std::make_shared<ConflictsAttribute>(hash);
            break;
        }
        default:
            throw DeserializationException("Unknown transaction attribute type: " + std::to_string(static_cast<int>(type)));
    }

    return attribute;
} // namespace neocpp
size_t OracleResponseAttribute::getSize() const {
    // Type byte + uint64 (id) + uint8 (code) + var bytes (result)
    size_t varIntSize = 1; // Default for values < 0xFD
    if (result_.size() >= 0xFD && result_.size() <= 0xFFFF) {
        varIntSize = 3;
    } else if (result_.size() > 0xFFFF && result_.size() <= 0xFFFFFFFF) {
        varIntSize = 5;
    } else if (result_.size() > 0xFFFFFFFF) {
        varIntSize = 9;
    }
    return 1 + 8 + 1 + varIntSize + result_.size();
} // namespace neocpp
void OracleResponseAttribute::serializeWithoutType(BinaryWriter& writer) const {
    writer.writeUInt64(id_);
    writer.writeUInt8(code_);
    writer.writeVarBytes(result_);
} // namespace neocpp
void NotValidBeforeAttribute::serializeWithoutType(BinaryWriter& writer) const {
    writer.writeUInt32(height_);
} // namespace neocpp
void ConflictsAttribute::serializeWithoutType(BinaryWriter& writer) const {
    hash_.serialize(writer);
} // namespace neocpp
} // namespace neocpp
