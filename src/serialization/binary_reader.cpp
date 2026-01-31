#include "neocpp/serialization/binary_reader.hpp"
#include "neocpp/exceptions.hpp"
#include <cstring>
#include <algorithm>
#include <limits>

namespace neocpp {

BinaryReader::BinaryReader(const Bytes& data)
    : data_(nullptr), size_(0), position_(0) {
    // Copy data to owned storage to avoid dangling pointer issues with temporaries
    ownedData_ = data;
    data_ = ownedData_.data();
    size_ = ownedData_.size();
}

BinaryReader::BinaryReader(const uint8_t* data, size_t size)
    : data_(data), size_(size), position_(0) {
}

BinaryReader::BinaryReader(std::istream& stream)
    : data_(nullptr), size_(0), position_(0) {
    // Read all data from stream
    ownedData_.assign(std::istreambuf_iterator<char>(stream),
                      std::istreambuf_iterator<char>());
    data_ = ownedData_.data();
    size_ = ownedData_.size();
} // namespace neocpp
uint8_t BinaryReader::readByte() {
    if (position_ >= size_) {
        throw DeserializationException("Attempted to read beyond end of data");
    }
    return data_[position_++];
} // namespace neocpp
bool BinaryReader::readBool() {
    return readByte() != 0;
} // namespace neocpp
Bytes BinaryReader::readBytes(size_t count) {
    if (position_ + count > size_) {
        throw DeserializationException("Attempted to read beyond end of data");
    }
    Bytes result(data_ + position_, data_ + position_ + count);
    position_ += count;
    return result;
} // namespace neocpp
void BinaryReader::readBytes(uint8_t* buffer, size_t count) {
    if (position_ + count > size_) {
        throw DeserializationException("Attempted to read beyond end of data");
    }
    std::memcpy(buffer, data_ + position_, count);
    position_ += count;
} // namespace neocpp
int8_t BinaryReader::readInt8() {
    return static_cast<int8_t>(readByte());
} // namespace neocpp
uint8_t BinaryReader::readUInt8() {
    return readByte();
} // namespace neocpp
int16_t BinaryReader::readInt16() {
    uint8_t b0 = readByte();
    uint8_t b1 = readByte();
    return static_cast<int16_t>(b0 | (b1 << 8));
} // namespace neocpp
uint16_t BinaryReader::readUInt16() {
    uint8_t b0 = readByte();
    uint8_t b1 = readByte();
    return static_cast<uint16_t>(b0 | (b1 << 8));
} // namespace neocpp
int32_t BinaryReader::readInt32() {
    uint8_t b0 = readByte();
    uint8_t b1 = readByte();
    uint8_t b2 = readByte();
    uint8_t b3 = readByte();
    return static_cast<int32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
} // namespace neocpp
uint32_t BinaryReader::readUInt32() {
    uint8_t b0 = readByte();
    uint8_t b1 = readByte();
    uint8_t b2 = readByte();
    uint8_t b3 = readByte();
    return static_cast<uint32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
} // namespace neocpp
int64_t BinaryReader::readInt64() {
    int64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= static_cast<int64_t>(readByte()) << (i * 8);
    }
    return result;
} // namespace neocpp
uint64_t BinaryReader::readUInt64() {
    uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= static_cast<uint64_t>(readByte()) << (i * 8);
    }
    return result;
} // namespace neocpp
uint64_t BinaryReader::readVarInt() {
    uint8_t first = readByte();
    if (first < 0xFD) {
        return first;
    } else if (first == 0xFD) {
        uint16_t val = readUInt16();
        if (val < 0xFD) {
            throw DeserializationException("Non-canonical varint encoding: value < 0xFD encoded with 0xFD prefix");
        }
        return val;
    } else if (first == 0xFE) {
        uint32_t val = readUInt32();
        if (val <= 0xFFFF) {
            throw DeserializationException("Non-canonical varint encoding: value <= 0xFFFF encoded with 0xFE prefix");
        }
        return val;
    } else {
        uint64_t val = readUInt64();
        if (val <= 0xFFFFFFFF) {
            throw DeserializationException("Non-canonical varint encoding: value <= 0xFFFFFFFF encoded with 0xFF prefix");
        }
        return val;
    }
} // namespace neocpp
Bytes BinaryReader::readVarBytes() {
    uint64_t length = readVarInt();
    if (length > std::numeric_limits<size_t>::max()) {
        throw DeserializationException("Data size exceeds platform limits");
    }
    return readBytes(static_cast<size_t>(length));
} // namespace neocpp
std::string BinaryReader::readVarString() {
    uint64_t length = readVarInt();
    Bytes bytes = readBytes(static_cast<size_t>(length));
    return std::string(bytes.begin(), bytes.end());
} // namespace neocpp
std::string BinaryReader::readFixedString(size_t length) {
    Bytes bytes = readBytes(length);
    // Find null terminator if present
    auto nullPos = std::find(bytes.begin(), bytes.end(), 0);
    if (nullPos != bytes.end()) {
        return std::string(bytes.begin(), nullPos);
    }
    return std::string(bytes.begin(), bytes.end());
} // namespace neocpp
void BinaryReader::skip(size_t count) {
    if (position_ + count > size_) {
        throw DeserializationException("Attempted to skip beyond end of data");
    }
    position_ += count;
} // namespace neocpp
void BinaryReader::seek(size_t position) {
    if (position > size_) {
        throw DeserializationException("Attempted to seek beyond end of data");
    }
    position_ = position;
} // namespace neocpp
} // namespace neocpp
