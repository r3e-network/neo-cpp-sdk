#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <istream>
#include <memory>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Binary reader for Neo deserialization
class BinaryReader {
private:
    const uint8_t* data_;
    size_t size_;
    size_t position_;
    std::vector<uint8_t> ownedData_;  // For stream-based construction

public:
    /// Constructor from byte array
    explicit BinaryReader(const Bytes& data);
    BinaryReader(const uint8_t* data, size_t size);
    
    /// Constructor from input stream (reads all data)
    explicit BinaryReader(std::istream& stream);

    ~BinaryReader() = default;

    /// Read a single byte
    uint8_t readByte();

    /// Read a boolean value
    bool readBool();

    /// Read bytes
    Bytes readBytes(size_t count);
    void readBytes(uint8_t* buffer, size_t count);

    /// Read integers (little-endian)
    int8_t readInt8();
    uint8_t readUInt8();
    int16_t readInt16();
    uint16_t readUInt16();
    int32_t readInt32();
    uint32_t readUInt32();
    int64_t readInt64();
    uint64_t readUInt64();

    /// Read variable length integer
    uint64_t readVarInt();

    /// Read variable length bytes
    Bytes readVarBytes();

    /// Read variable length string
    std::string readVarString();

    /// Read fixed length string
    std::string readFixedString(size_t length);

    /// Template read method for basic types
    template<typename T>
    T read() {
        static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");
        return readArithmetic<T>();
    }
    
    /// Read a deserializable object
    template<typename T>
    T readSerializable() {
        return T::deserialize(*this);
    }

    /// Read an array of deserializable objects
    template<typename T>
    std::vector<T> readSerializableArray() {
        uint64_t count = readVarInt();
        std::vector<T> result;
        result.reserve(count);
        for (uint64_t i = 0; i < count; ++i) {
            result.push_back(T::deserialize(*this));
        }
        return result;
    }

    /// Check if more data is available
    bool hasMore() const { return position_ < size_; }

    /// Get remaining bytes count
    size_t remaining() const { return size_ - position_; }

    /// Get current position
    size_t position() const { return position_; }

    /// Skip bytes
    void skip(size_t count);

    /// Reset position to beginning
    void reset() { position_ = 0; }

    /// Seek to specific position
    void seek(size_t position);
    
private:
    template<typename T>
    T readArithmetic() {
        static_assert(sizeof(T) <= 8, "Type too large");
        T value = 0;
        if (position_ + sizeof(T) > size_) {
            throw std::runtime_error("Not enough data to read");
        }
        // Little-endian read
        for (size_t i = 0; i < sizeof(T); ++i) {
            value |= static_cast<T>(data_[position_ + i]) << (8 * i);
        }
        position_ += sizeof(T);
        return value;
    }
};

} // namespace neocpp
