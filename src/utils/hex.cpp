
#include "neocpp/utils/hex.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

namespace neocpp {

std::string Hex::encode(const Bytes& data, bool uppercase) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    if (uppercase) {
        ss << std::uppercase;
    }

    for (const auto& byte : data) {
        ss << std::setw(2) << static_cast<int>(byte);
    }

    return ss.str();
} // namespace neocpp
Bytes Hex::decode(const std::string& hex) {
    std::string cleanHex = withoutPrefix(hex);

    // Return empty for odd-length strings
    if (cleanHex.length() % 2 != 0) {
        return Bytes();
    }

    Bytes result;
    result.reserve(cleanHex.length() / 2);

    for (size_t i = 0; i < cleanHex.length(); i += 2) {
        std::string byteString = cleanHex.substr(i, 2);

        // Validate that both characters are valid hex
        for (char c : byteString) {
            if (!((c >= '0' && c <= '9') ||
                  (c >= 'a' && c <= 'f') ||
                  (c >= 'A' && c <= 'F'))) {
                // Return empty for invalid hex characters
                return Bytes();
            }
        }

        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        result.push_back(byte);
    }

    return result;
} // namespace neocpp
bool Hex::isValid(const std::string& str) {
    // Special case: just "0x" or "0X" with no data is invalid
    if (str == "0x" || str == "0X") {
        return false;
    }

    std::string cleanStr = withoutPrefix(str);

    if (cleanStr.empty()) {
        // Empty string (no prefix) is valid, but "0x" alone is not
        return str.empty() || (str != "0x" && str != "0X");
    }

    // Check for odd length (invalid hex must have even number of characters)
    if (cleanStr.length() % 2 != 0) {
        return false;
    }

    // Check all characters are valid hex
    return std::all_of(cleanStr.begin(), cleanStr.end(), [](char c) {
        return (c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'f') ||
               (c >= 'A' && c <= 'F');
    });
} // namespace neocpp
std::string Hex::withPrefix(const std::string& hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        return hex;
    }
    return "0x" + hex;
} // namespace neocpp
std::string Hex::withoutPrefix(const std::string& hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        return hex.substr(2);
    }
    return hex;
} // namespace neocpp
} // namespace neocpp
