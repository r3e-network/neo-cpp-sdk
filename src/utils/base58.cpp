#include "neocpp/utils/base58.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/exceptions.hpp"
#include <algorithm>
#include <vector>
#include <cstring>

namespace neocpp {

const char* Base58::ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const int Base58::BASE = 58;

std::string Base58::encode(const Bytes& data) {
    if (data.empty()) {
        return "";
    }

    // Count leading zeros
    size_t zeros = 0;
    for (const auto& byte : data) {
        if (byte == 0) {
            zeros++;
        } else {
            break;
        }
    }

    // Allocate enough space
    size_t size = (data.size() * 138 / 100) + 1;
    std::vector<uint8_t> buffer(size);

    size_t length = 0;
    for (const auto& byte : data) {
        int carry = byte;
        for (size_t i = 0; i < length || carry; ++i) {
            carry += 256 * buffer[i];
            buffer[i] = carry % BASE;
            carry /= BASE;
            if (i >= length) {
                length = i + 1;
            }
        }
    }

    // Build result string
    std::string result;
    result.reserve(zeros + length);

    for (size_t i = 0; i < zeros; ++i) {
        result += ALPHABET[0];
    }

    for (size_t i = 0; i < length; ++i) {
        result += ALPHABET[buffer[length - 1 - i]];
    }

    return result;
} // namespace neocpp
Bytes Base58::decode(const std::string& encoded) {
    if (encoded.empty()) {
        return Bytes();
    }

    // Count leading '1's
    size_t zeros = 0;
    for (const auto& c : encoded) {
        if (c == ALPHABET[0]) {
            zeros++;
        } else {
            break;
        }
    }

    // Decode
    size_t size = encoded.size() * 733 / 1000 + 1;
    std::vector<uint8_t> buffer(size);
    size_t length = 0;

    for (const auto& c : encoded) {
        const char* p = std::strchr(ALPHABET, c);
        if (p == nullptr) {
            // Return empty bytes for invalid characters instead of throwing
            return Bytes();
        }

        int carry = p - ALPHABET;
        for (size_t i = 0; i < length || carry; ++i) {
            carry += BASE * buffer[i];
            buffer[i] = carry % 256;
            carry /= 256;
            if (i >= length) {
                length = i + 1;
            }
        }
    }

    // Build result
    Bytes result;
    result.reserve(zeros + length);

    for (size_t i = 0; i < zeros; ++i) {
        result.push_back(0);
    }

    for (size_t i = 0; i < length; ++i) {
        result.push_back(buffer[length - 1 - i]);
    }

    return result;
} // namespace neocpp
std::string Base58::encodeCheck(const Bytes& data) {
    Bytes checksum = calculateChecksum(data);
    Bytes dataWithChecksum = data;
    dataWithChecksum.insert(dataWithChecksum.end(), checksum.begin(), checksum.end());
    return encode(dataWithChecksum);
} // namespace neocpp
Bytes Base58::decodeCheck(const std::string& encoded) {
    Bytes decoded = decode(encoded);

    // Return empty if decode failed or too short
    if (decoded.empty() || decoded.size() < 4) {
        return Bytes();
    }

    // Return empty if checksum is invalid
    if (!verifyChecksum(decoded)) {
        return Bytes();
    }

    return Bytes(decoded.begin(), decoded.end() - 4);
} // namespace neocpp
Bytes Base58::calculateChecksum(const Bytes& data) {
    Bytes hash = HashUtils::doubleSha256(data);
    return Bytes(hash.begin(), hash.begin() + 4);
} // namespace neocpp
bool Base58::verifyChecksum(const Bytes& dataWithChecksum) {
    if (dataWithChecksum.size() < 4) {
        return false;
    }

    Bytes data(dataWithChecksum.begin(), dataWithChecksum.end() - 4);
    Bytes checksum(dataWithChecksum.end() - 4, dataWithChecksum.end());
    Bytes calculatedChecksum = calculateChecksum(data);

    return std::equal(checksum.begin(), checksum.end(), calculatedChecksum.begin());
} // namespace neocpp
} // namespace neocpp
