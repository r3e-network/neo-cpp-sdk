
#include "neocpp/crypto/nep2.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/base58.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/logger.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <cstring>

namespace neocpp {

// Scrypt implementation
static Bytes scrypt(const Bytes& password, const Bytes& salt, const ScryptParams& params) {
    Bytes result(params.getDkLen());

    // OpenSSL's EVP_PBE_scrypt
    if (EVP_PBE_scrypt(reinterpret_cast<const char*>(password.data()), password.size(),
                       salt.data(), salt.size(),
                       params.getN(), params.getR(), params.getP(),
                       0, result.data(), result.size()) != 1) {
        throw CryptoException("Scrypt derivation failed");
    }

    return result;
} // namespace neocpp
// AES-256-ECB encryption
static Bytes aesEncrypt(const Bytes& data, const Bytes& key) {
    if (key.size() != 32) {
        throw CryptoException("AES key must be 32 bytes");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw CryptoException("Failed to create cipher context");
    }

    Bytes result(data.size());
    int outLen = 0;
    int finalLen = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), nullptr, key.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to initialize AES encryption");
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0); // No padding for ECB mode

    if (EVP_EncryptUpdate(ctx, result.data(), &outLen, data.data(), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to encrypt data");
    }

    if (EVP_EncryptFinal_ex(ctx, result.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to finalize encryption");
    }

    EVP_CIPHER_CTX_free(ctx);
    result.resize(outLen + finalLen);
    return result;
} // namespace neocpp
// AES-256-ECB decryption
static Bytes aesDecrypt(const Bytes& data, const Bytes& key) {
    if (key.size() != 32) {
        throw CryptoException("AES key must be 32 bytes");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw CryptoException("Failed to create cipher context");
    }

    Bytes result(data.size());
    int outLen = 0;
    int finalLen = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ecb(), nullptr, key.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to initialize AES decryption");
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0); // No padding for ECB mode

    if (EVP_DecryptUpdate(ctx, result.data(), &outLen, data.data(), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to decrypt data");
    }

    if (EVP_DecryptFinal_ex(ctx, result.data() + outLen, &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw CryptoException("Failed to finalize decryption");
    }

    EVP_CIPHER_CTX_free(ctx);
    result.resize(outLen + finalLen);
    return result;
} // namespace neocpp
std::string NEP2::encrypt(const Bytes& privateKey, const std::string& password, const ScryptParams& params) {
    if (privateKey.size() != 32) {
        throw NEP2Exception("Private key must be 32 bytes");
    }

    // Get address for salt
    ECKeyPair keyPair(privateKey);
    std::string address = keyPair.getAddress();
    Bytes addressHash = HashUtils::sha256(HashUtils::sha256(Bytes(address.begin(), address.end())));
    Bytes salt(addressHash.begin(), addressHash.begin() + 4);

    // Derive key using scrypt
    Bytes passwordBytes(password.begin(), password.end());
    Bytes derivedKey = scrypt(passwordBytes, salt, params);

    // Split derived key
    Bytes encryptKey(derivedKey.begin(), derivedKey.begin() + 32);
    Bytes derivedKey2(derivedKey.begin() + 32, derivedKey.end());

    // XOR private key halves with derivedKey2 (not addressHash)
    Bytes xor1(privateKey.begin(), privateKey.begin() + 16);
    Bytes xor2(privateKey.begin() + 16, privateKey.end());
    for (size_t i = 0; i < 16; ++i) {
        xor1[i] ^= derivedKey2[i];
        xor2[i] ^= derivedKey2[i + 16];
    }

    // Encrypt
    Bytes encrypted1 = aesEncrypt(xor1, encryptKey);
    Bytes encrypted2 = aesEncrypt(xor2, encryptKey);

    // Build NEP-2 encrypted key
    Bytes nep2;
    nep2.push_back(NEP2_PREFIX_1);
    nep2.push_back(NEP2_PREFIX_2);
    nep2.push_back(NEP2_FLAG);
    nep2.insert(nep2.end(), salt.begin(), salt.end());
    nep2.insert(nep2.end(), encrypted1.begin(), encrypted1.end());
    nep2.insert(nep2.end(), encrypted2.begin(), encrypted2.end());

    return Base58::encodeCheck(nep2);
} // namespace neocpp
std::string NEP2::encrypt(const ECKeyPair& keyPair, const std::string& password, const ScryptParams& params) {
    return encrypt(keyPair.getPrivateKey()->getBytes(), password, params);
} // namespace neocpp
Bytes NEP2::decrypt(const std::string& nep2, const std::string& password, const ScryptParams& params) {
    if (!isValid(nep2)) {
        throw NEP2Exception("Invalid NEP-2 format");
    }

    Bytes encrypted = Base58::decodeCheck(nep2);
    if (encrypted.size() != NEP2_ENCRYPTED_SIZE) {
        throw NEP2Exception("Invalid NEP-2 length");
    }

    // Extract components
    Bytes salt(encrypted.begin() + 3, encrypted.begin() + 7);
    Bytes encrypted1(encrypted.begin() + 7, encrypted.begin() + 23);
    Bytes encrypted2(encrypted.begin() + 23, encrypted.begin() + 39);

    // Derive key using scrypt
    Bytes passwordBytes(password.begin(), password.end());
    Bytes derivedKey = scrypt(passwordBytes, salt, params);

    // Split derived key
    Bytes decryptKey(derivedKey.begin(), derivedKey.begin() + 32);

    // Decrypt
    Bytes decrypted1 = aesDecrypt(encrypted1, decryptKey);
    Bytes decrypted2 = aesDecrypt(encrypted2, decryptKey);

    // According to NEP-2:
    // The second half of the derived key is used for verification
    Bytes derivedKey2(derivedKey.begin() + 32, derivedKey.end());

    // XOR decrypted halves with derivedKey2 to get the intermediate
    Bytes intermediate1(16);
    Bytes intermediate2(16);
    for (size_t i = 0; i < 16; ++i) {
        intermediate1[i] = decrypted1[i] ^ derivedKey2[i];
        intermediate2[i] = decrypted2[i] ^ derivedKey2[i + 16];
    }

    // Combine to get private key
    Bytes privateKey;
    privateKey.insert(privateKey.end(), intermediate1.begin(), intermediate1.end());
    privateKey.insert(privateKey.end(), intermediate2.begin(), intermediate2.end());

    // Verify by checking address
    try {
        ECKeyPair keyPair(privateKey);
        std::string address = keyPair.getAddress();
        Bytes addressHash = HashUtils::sha256(HashUtils::sha256(Bytes(address.begin(), address.end())));

        // Verify salt matches using constant-time comparison
        if (!HashUtils::constantTimeEquals(salt.data(), addressHash.data(), 4)) {
            throw NEP2Exception("Invalid password");
        }

        return privateKey;
    } catch (const NEP2Exception&) {
        throw;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("NEP-2 decryption failed: ") + e.what());
        throw NEP2Exception("Invalid password or corrupted NEP-2 key");
    } catch (...) {
        LOG_DEBUG("NEP-2 decryption failed with unknown error");
        throw NEP2Exception("Invalid password or corrupted NEP-2 key");
    }
} // namespace neocpp
ECKeyPair NEP2::decryptToKeyPair(const std::string& nep2, const std::string& password, const ScryptParams& params) {
    Bytes privateKey = decrypt(nep2, password, params);
    return ECKeyPair(privateKey);
} // namespace neocpp
bool NEP2::isValid(const std::string& nep2) {
    if (nep2.length() != 58) {
        return false;
    }

    try {
        Bytes decoded = Base58::decodeCheck(nep2);
        if (decoded.empty()) {
            return false;
        }
        return decoded.size() == NEP2_ENCRYPTED_SIZE &&
               decoded[0] == NEP2_PREFIX_1 &&
               decoded[1] == NEP2_PREFIX_2 &&
               decoded[2] == NEP2_FLAG;
    } catch (const std::exception& e) {
        LOG_DEBUG(std::string("NEP-2 validation failed: ") + e.what());
        return false;
    } catch (...) {
        LOG_DEBUG("NEP-2 validation failed with unknown error");
        return false;
    }
} // namespace neocpp
std::string NEP2::getAddress(const std::string& nep2) {
    if (!isValid(nep2)) {
        throw NEP2Exception("Invalid NEP-2 format");
    }

    // NEP-2 doesn't directly contain the address
    // We'd need to decrypt to get it
    throw NEP2Exception("Cannot get address without decryption");
} // namespace neocpp
} // namespace neocpp
