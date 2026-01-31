
#include "neocpp/serialization/binary_writer.hpp"
#include <cstring>

namespace neocpp {

void BinaryWriter::writeByte(uint8_t value) {
    if (stream_) {
        stream_->write(reinterpret_cast<const char*>(&value), 1);
    } else {
        buffer_.push_back(value);
    }
} // namespace neocpp
void BinaryWriter::writeBool(bool value) {
    writeByte(value ? 1 : 0);
} // namespace neocpp
void BinaryWriter::writeBytes(const Bytes& bytes) {
    if (stream_) {
        stream_->write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    } else {
        buffer_.insert(buffer_.end(), bytes.begin(), bytes.end());
    }
} // namespace neocpp
void BinaryWriter::writeBytes(const uint8_t* data, size_t length) {
    if (stream_) {
        stream_->write(reinterpret_cast<const char*>(data), length);
    } else {
        buffer_.insert(buffer_.end(), data, data + length);
    }
} // namespace neocpp
void BinaryWriter::writeInt8(int8_t value) {
    writeByte(static_cast<uint8_t>(value));
} // namespace neocpp
void BinaryWriter::writeUInt8(uint8_t value) {
    writeByte(value);
} // namespace neocpp
void BinaryWriter::writeInt16(int16_t value) {
    writeByte(value & 0xFF);
    writeByte((value >> 8) & 0xFF);
} // namespace neocpp
void BinaryWriter::writeUInt16(uint16_t value) {
    writeByte(value & 0xFF);
    writeByte((value >> 8) & 0xFF);
} // namespace neocpp
void BinaryWriter::writeInt32(int32_t value) {
    writeByte(value & 0xFF);
    writeByte((value >> 8) & 0xFF);
    writeByte((value >> 16) & 0xFF);
    writeByte((value >> 24) & 0xFF);
} // namespace neocpp
void BinaryWriter::writeUInt32(uint32_t value) {
    writeByte(value & 0xFF);
    writeByte((value >> 8) & 0xFF);
    writeByte((value >> 16) & 0xFF);
    writeByte((value >> 24) & 0xFF);
} // namespace neocpp
void BinaryWriter::writeInt64(int64_t value) {
    for (int i = 0; i < 8; ++i) {
        writeByte((value >> (i * 8)) & 0xFF);
    }
} // namespace neocpp
void BinaryWriter::writeUInt64(uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        writeByte((value >> (i * 8)) & 0xFF);
    }
} // namespace neocpp
void BinaryWriter::writeVarInt(uint64_t value) {
    if (value < 0xFD) {
        writeByte(static_cast<uint8_t>(value));
    } else if (value <= 0xFFFF) {
        writeByte(0xFD);
        writeUInt16(static_cast<uint16_t>(value));
    } else if (value <= 0xFFFFFFFF) {
        writeByte(0xFE);
        writeUInt32(static_cast<uint32_t>(value));
    } else {
        writeByte(0xFF);
        writeUInt64(value);
    }
} // namespace neocpp
void BinaryWriter::writeVarBytes(const Bytes& bytes) {
    writeVarInt(bytes.size());
    writeBytes(bytes);
} // namespace neocpp
void BinaryWriter::writeVarString(const std::string& str) {
    writeVarInt(str.size());
    writeBytes(reinterpret_cast<const uint8_t*>(str.data()), str.size());
} // namespace neocpp
void BinaryWriter::writeFixedString(const std::string& str, size_t length) {
    size_t writeLength = std::min(str.size(), length);
    writeBytes(reinterpret_cast<const uint8_t*>(str.data()), writeLength);

    // Pad with zeros if necessary
    for (size_t i = writeLength; i < length; ++i) {
        writeByte(0);
    }
} // namespace neocpp
} // namespace neocpp
