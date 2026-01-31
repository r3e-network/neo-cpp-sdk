
#include "neocpp/utils/base64.hpp"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <memory>

namespace neocpp {

std::string Base64::encode(const Bytes& data) {
    if (data.empty()) {
        return "";
    }

    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);

    BUF_MEM* bufferPtr = nullptr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return result;
} // namespace neocpp
Bytes Base64::decode(const std::string& encoded) {
    if (encoded.empty()) {
        return Bytes();
    }

    // Validate input - length must be multiple of 4
    if (encoded.length() % 4 != 0) {
        return Bytes();
    }

    // Check for valid padding placement
    size_t paddingPos = encoded.find('=');
    if (paddingPos != std::string::npos) {
        // Once we see padding, all characters after must also be padding
        for (size_t i = paddingPos; i < encoded.length(); ++i) {
            if (encoded[i] != '=') {
                return Bytes();  // Invalid: non-padding after padding
            }
        }
        // Maximum 2 padding characters allowed
        size_t paddingCount = encoded.length() - paddingPos;
        if (paddingCount > 2) {
            return Bytes();  // Too many padding characters
        }
    }

    // Check for invalid characters
    for (char c : encoded) {
        if (!( (c >= 'A' && c <= 'Z') ||
               (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') ||
               c == '+' || c == '/' || c == '=' ||
               c == '-' || c == '_' )) {
            return Bytes();  // Invalid character
        }
    }

    BIO* bio = BIO_new_mem_buf(encoded.data(), encoded.length());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    Bytes result(encoded.length());
    int decodedLength = BIO_read(bio, result.data(), encoded.length());
    BIO_free_all(bio);

    if (decodedLength < 0) {
        return Bytes();
    }

    result.resize(decodedLength);
    return result;
} // namespace neocpp
bool Base64::isValid(const std::string& str) {
    if (str.empty()) {
        return true;
    }

    // Base64 strings must have length that's a multiple of 4
    if (str.length() % 4 != 0) {
        return false;
    }

    for (char c : str) {
        if (!((c >= 'A' && c <= 'Z') ||
              (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') ||
              c == '+' || c == '/' || c == '=' ||
              c == '-' || c == '_')) {  // URL-safe characters
            return false;
        }
    }

    // Check padding
    size_t paddingStart = str.find('=');
    if (paddingStart != std::string::npos) {
        // Padding can only be at the end
        for (size_t i = paddingStart; i < str.length(); ++i) {
            if (str[i] != '=') {
                return false;
            }
        }
        // Check valid padding amount (max 2 '=' at end)
        size_t paddingCount = str.length() - paddingStart;
        if (paddingCount > 2) {
            return false;
        }
    }

    return true;
} // namespace neocpp
} // namespace neocpp
