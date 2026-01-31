#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Binary writer for Neo serialization
class BinaryWriter {
private:
    std::vector<uint8_t> buffer_;
    std::ostream* stream_;

public:
    BinaryWriter() : stream_(nullptr) {}
    explicit BinaryWriter(std::ostream& stream) : stream_(&stream) {}
    ~BinaryWriter() = default;

    /// Write a single byte
    void writeByte(uint8_t value);

    /// Write a boolean value
    void writeBool(bool value);

    /// Write bytes
    void writeBytes(const Bytes& bytes);
    void writeBytes(const uint8_t* data, size_t length);

    /// Write integers (little-endian)
    void writeInt8(int8_t value);
    void writeUInt8(uint8_t value);
    void writeInt16(int16_t value);
    void writeUInt16(uint16_t value);
    void writeInt32(int32_t value);
    void writeUInt32(uint32_t value);
    void writeInt64(int64_t value);
    void writeUInt64(uint64_t value);

    /// Write variable length integer
    void writeVarInt(uint64_t value);

    /// Write variable length bytes
    void writeVarBytes(const Bytes& bytes);

    /// Write variable length string
    void writeVarString(const std::string& str);

    /// Write fixed length string
    void writeFixedString(const std::string& str, size_t length);

    /// Template write method for basic types
    template<typename T>
    void write(T value) {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "T must be an arithmetic or enum type");
        writeArithmetic(value);
    }
    
    /// Write boolean (special case)
    void write(bool value) {
        writeBool(value);
    }
    
    /// Write bytes vector (special case)
    void write(const Bytes& value) {
        writeBytes(value);
    }
    
    /// Write a serializable object
    template<typename T>
    void writeSerializable(const T& obj) {
        obj.serialize(*this);
    }

    /// Write an array of serializable objects
    template<typename T>
    void writeSerializableArray(const std::vector<T>& array) {
        writeVarInt(array.size());
        for (const auto& item : array) {
            item.serialize(*this);
        }
    }

    /// Get the written bytes
    const Bytes& toArray() const { return buffer_; }

    /// Get the current size of the buffer
    size_t size() const { return buffer_.size(); }

    /// Clear the buffer
    void clear() { buffer_.clear(); }

    /// Reserve capacity
    void reserve(size_t capacity) { buffer_.reserve(capacity); }
    
    /// Get the size of a variable length integer
    static size_t getVarSize(uint64_t value) {
        if (value < 0xFD) {
            return 1;
        } else if (value <= 0xFFFF) {
            return 3;
        } else if (value <= 0xFFFFFFFF) {
            return 5;
        } else {
            return 9;
        }
    }
    
    /// Get the size of a variable length string
    static size_t getVarSize(const std::string& str) {
        return getVarSize(str.size()) + str.size();
    }
    
private:
    template<typename T>
    void writeArithmetic(T value) {
        static_assert(sizeof(T) <= 8, "Type too large");
        // Little-endian write
        for (size_t i = 0; i < sizeof(T); ++i) {
            writeByte(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
};

} // namespace neocpp
