
#include "neocpp/types/types.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace neocpp {

std::string ByteUtils::toHex(const Bytes& bytes, bool with_prefix) {
    std::stringstream ss;
    if (with_prefix) {
        ss << "0x";
    }
    for (const auto& byte : bytes) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
} // namespace neocpp
Bytes ByteUtils::fromHex(const std::string& hex) {
    std::string cleanHex = hex;
    // Remove 0x prefix if present
    if (cleanHex.size() >= 2 && cleanHex[0] == '0' && (cleanHex[1] == 'x' || cleanHex[1] == 'X')) {
        cleanHex = cleanHex.substr(2);
    }

    // Ensure even length
    if (cleanHex.length() % 2 != 0) {
        cleanHex = "0" + cleanHex;
    }

    Bytes result;
    result.reserve(cleanHex.length() / 2);

    for (size_t i = 0; i < cleanHex.length(); i += 2) {
        std::string byteString = cleanHex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        result.push_back(byte);
    }

    return result;
} // namespace neocpp
Bytes ByteUtils::reverse(const Bytes& bytes) {
    Bytes result(bytes.rbegin(), bytes.rend());
    return result;
} // namespace neocpp
bool ByteUtils::equal(const Bytes& a, const Bytes& b) {
    if (a.size() != b.size()) {
        return false;
    }
    return std::equal(a.begin(), a.end(), b.begin());
} // namespace neocpp
Bytes ByteUtils::concat(const std::vector<Bytes>& arrays) {
    Bytes result;
    size_t totalSize = 0;
    for (const auto& array : arrays) {
        totalSize += array.size();
    }
    result.reserve(totalSize);

    for (const auto& array : arrays) {
        result.insert(result.end(), array.begin(), array.end());
    }
    return result;
} // namespace neocpp
Bytes ByteUtils::subset(const Bytes& bytes, size_t start, size_t length) {
    if (start >= bytes.size()) {
        return Bytes();
    }

    size_t actualLength = std::min(length, bytes.size() - start);
    return Bytes(bytes.begin() + start, bytes.begin() + start + actualLength);
} // namespace neocpp
Bytes ByteUtils::fromInt32LE(int32_t value) {
    Bytes result(4);
    result[0] = value & 0xFF;
    result[1] = (value >> 8) & 0xFF;
    result[2] = (value >> 16) & 0xFF;
    result[3] = (value >> 24) & 0xFF;
    return result;
} // namespace neocpp
Bytes ByteUtils::fromInt64LE(int64_t value) {
    Bytes result(8);
    for (int i = 0; i < 8; ++i) {
        result[i] = (value >> (i * 8)) & 0xFF;
    }
    return result;
} // namespace neocpp
Bytes ByteUtils::fromInt32BE(int32_t value) {
    Bytes result(4);
    result[0] = (value >> 24) & 0xFF;
    result[1] = (value >> 16) & 0xFF;
    result[2] = (value >> 8) & 0xFF;
    result[3] = value & 0xFF;
    return result;
} // namespace neocpp
Bytes ByteUtils::fromInt64BE(int64_t value) {
    Bytes result(8);
    for (int i = 0; i < 8; ++i) {
        result[i] = (value >> ((7 - i) * 8)) & 0xFF;
    }
    return result;
} // namespace neocpp
int32_t ByteUtils::toInt32LE(const Bytes& bytes) {
    if (bytes.size() < 4) {
        throw std::invalid_argument("Insufficient bytes for int32");
    }
    return static_cast<int32_t>(bytes[0]) |
           (static_cast<int32_t>(bytes[1]) << 8) |
           (static_cast<int32_t>(bytes[2]) << 16) |
           (static_cast<int32_t>(bytes[3]) << 24);
} // namespace neocpp
int64_t ByteUtils::toInt64LE(const Bytes& bytes) {
    if (bytes.size() < 8) {
        throw std::invalid_argument("Insufficient bytes for int64");
    }
    int64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= static_cast<int64_t>(bytes[i]) << (i * 8);
    }
    return result;
} // namespace neocpp
int32_t ByteUtils::toInt32BE(const Bytes& bytes) {
    if (bytes.size() < 4) {
        throw std::invalid_argument("Insufficient bytes for int32");
    }
    return (static_cast<int32_t>(bytes[0]) << 24) |
           (static_cast<int32_t>(bytes[1]) << 16) |
           (static_cast<int32_t>(bytes[2]) << 8) |
           static_cast<int32_t>(bytes[3]);
} // namespace neocpp
int64_t ByteUtils::toInt64BE(const Bytes& bytes) {
    if (bytes.size() < 8) {
        throw std::invalid_argument("Insufficient bytes for int64");
    }
    int64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result |= static_cast<int64_t>(bytes[i]) << ((7 - i) * 8);
    }
    return result;
} // namespace neocpp
} // namespace neocpp
