
#include "neocpp/crypto/bip32_ec_key_pair.hpp"
#include "neocpp/crypto/bip39.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/base58.hpp"
#include "neocpp/exceptions.hpp"
#include <openssl/hmac.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <sstream>
#include <cstring>

namespace neocpp {

// BIP32 constants
static const std::string BITCOIN_SEED = "Bitcoin seed";
static const uint32_t HARDENED_BIT = 0x80000000;

// Version bytes for extended keys
static const uint32_t MAINNET_PRIVATE = 0x0488ADE4;
static const uint32_t MAINNET_PUBLIC = 0x0488B21E;

Bip32ECKeyPair::Bip32ECKeyPair(const SharedPtr<ECPrivateKey>& privateKey,
                               const Bytes& chainCode,
                               uint32_t depth,
                               uint32_t parentFingerprint,
                               uint32_t childNumber)
    : ECKeyPair(privateKey),
      chainCode_(chainCode),
      depth_(depth),
      parentFingerprint_(parentFingerprint),
      childNumber_(childNumber) {
} // namespace neocpp
SharedPtr<Bip32ECKeyPair> Bip32ECKeyPair::fromSeed(const Bytes& seed) {
    if (seed.size() < 16 || seed.size() > 64) {
        throw IllegalArgumentException("Seed must be between 16 and 64 bytes");
    }

    // HMAC-SHA512 with "Bitcoin seed" as key
    Bytes hmacKey(BITCOIN_SEED.begin(), BITCOIN_SEED.end());
    Bytes hmacResult(64);

    unsigned int len = 64;
    HMAC(EVP_sha512(), hmacKey.data(), hmacKey.size(),
         seed.data(), seed.size(), hmacResult.data(), &len);

    // Split result: first 32 bytes = private key, last 32 bytes = chain code
    Bytes privateKeyBytes(hmacResult.begin(), hmacResult.begin() + 32);
    Bytes chainCode(hmacResult.begin() + 32, hmacResult.end());

    // Create private key
    auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);

    return std::make_shared<Bip32ECKeyPair>(privateKey, chainCode, 0, 0, 0);
} // namespace neocpp
SharedPtr<Bip32ECKeyPair> Bip32ECKeyPair::fromMnemonic(const std::string& mnemonic,
                                                       const std::string& passphrase) {
    // Use the BIP39 implementation to convert mnemonic to seed
    Bytes seed = Bip39::mnemonicToSeed(mnemonic, passphrase);
    return fromSeed(seed);
} // namespace neocpp
SharedPtr<Bip32ECKeyPair> Bip32ECKeyPair::deriveChild(uint32_t index, bool hardened) {
    if (hardened) {
        index |= HARDENED_BIT;
    }

    Bytes data;
    data.reserve(37);

    if (hardened) {
        // Hardened derivation: 0x00 || private key || index
        data.push_back(0x00);
        Bytes privKey = getPrivateKey()->getBytes();
        data.insert(data.end(), privKey.begin(), privKey.end());
    } else {
        // Non-hardened derivation: public key || index
        Bytes pubKey = getPublicKey()->getEncoded();
        data.insert(data.end(), pubKey.begin(), pubKey.end());
    }

    // Append index (big-endian)
    data.push_back((index >> 24) & 0xFF);
    data.push_back((index >> 16) & 0xFF);
    data.push_back((index >> 8) & 0xFF);
    data.push_back(index & 0xFF);

    // HMAC-SHA512 with chain code as key
    Bytes hmacResult(64);
    unsigned int len = 64;
    HMAC(EVP_sha512(), chainCode_.data(), chainCode_.size(),
         data.data(), data.size(), hmacResult.data(), &len);

    // Split result
    Bytes childPrivateKeyBytes(hmacResult.begin(), hmacResult.begin() + 32);
    Bytes childChainCode(hmacResult.begin() + 32, hmacResult.end());

    // Add parent private key to child key (modulo curve order)
    BIGNUM* parentKey = BN_new();
    BIGNUM* childKey = BN_new();
    BIGNUM* curveOrder = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    // Get curve order
    EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    EC_GROUP_get_order(group, curveOrder, ctx);

    // Convert keys to BIGNUMs
    BN_bin2bn(getPrivateKey()->getBytes().data(), 32, parentKey);
    BN_bin2bn(childPrivateKeyBytes.data(), 32, childKey);

    // Add and mod
    BN_mod_add(childKey, parentKey, childKey, curveOrder, ctx);

    // Convert back to bytes
    Bytes finalKey(32);
    BN_bn2binpad(childKey, finalKey.data(), 32);

    // Cleanup
    BN_free(parentKey);
    BN_free(childKey);
    BN_free(curveOrder);
    BN_CTX_free(ctx);
    EC_GROUP_free(group);

    // Calculate parent fingerprint (first 4 bytes of parent public key hash)
    Bytes parentPubKey = getPublicKey()->getEncoded();
    Bytes parentHash = HashUtils::sha256ThenRipemd160(parentPubKey);
    uint32_t fingerprint = (parentHash[0] << 24) | (parentHash[1] << 16) |
                          (parentHash[2] << 8) | parentHash[3];

    // Create child key
    auto childPrivateKey = std::make_shared<ECPrivateKey>(finalKey);
    return std::make_shared<Bip32ECKeyPair>(childPrivateKey, childChainCode,
                                           depth_ + 1, fingerprint, index);
} // namespace neocpp
SharedPtr<Bip32ECKeyPair> Bip32ECKeyPair::derivePath(const std::string& path) {
    // Parse BIP32 path like "m/44'/888'/0'/0/0"
    if (path.empty() || path[0] != 'm') {
        throw IllegalArgumentException("Path must start with 'm'");
    }

    std::istringstream iss(path.substr(1));
    std::string segment;
    // Start with a copy of this key
    SharedPtr<Bip32ECKeyPair> current(new Bip32ECKeyPair(*this));

    while (std::getline(iss, segment, '/')) {
        if (segment.empty()) continue;

        bool hardened = false;
        if (segment.back() == '\'' || segment.back() == 'h') {
            hardened = true;
            segment.pop_back();
        }

        uint32_t index = std::stoul(segment);
        current = current->deriveChild(index, hardened);
    }

    return current;
} // namespace neocpp
std::string Bip32ECKeyPair::toExtendedPrivateKey() const {
    Bytes data;
    data.reserve(78);

    // Version (4 bytes)
    data.push_back((MAINNET_PRIVATE >> 24) & 0xFF);
    data.push_back((MAINNET_PRIVATE >> 16) & 0xFF);
    data.push_back((MAINNET_PRIVATE >> 8) & 0xFF);
    data.push_back(MAINNET_PRIVATE & 0xFF);

    // Depth (1 byte)
    data.push_back(depth_);

    // Parent fingerprint (4 bytes)
    data.push_back((parentFingerprint_ >> 24) & 0xFF);
    data.push_back((parentFingerprint_ >> 16) & 0xFF);
    data.push_back((parentFingerprint_ >> 8) & 0xFF);
    data.push_back(parentFingerprint_ & 0xFF);

    // Child number (4 bytes)
    data.push_back((childNumber_ >> 24) & 0xFF);
    data.push_back((childNumber_ >> 16) & 0xFF);
    data.push_back((childNumber_ >> 8) & 0xFF);
    data.push_back(childNumber_ & 0xFF);

    // Chain code (32 bytes)
    data.insert(data.end(), chainCode_.begin(), chainCode_.end());

    // Private key (33 bytes: 0x00 prefix + 32 byte key)
    data.push_back(0x00);
    Bytes privKey = getPrivateKey()->getBytes();
    data.insert(data.end(), privKey.begin(), privKey.end());

    // Base58Check encode
    return Base58::encode(data);
} // namespace neocpp
std::string Bip32ECKeyPair::toExtendedPublicKey() const {
    Bytes data;
    data.reserve(78);

    // Version (4 bytes)
    data.push_back((MAINNET_PUBLIC >> 24) & 0xFF);
    data.push_back((MAINNET_PUBLIC >> 16) & 0xFF);
    data.push_back((MAINNET_PUBLIC >> 8) & 0xFF);
    data.push_back(MAINNET_PUBLIC & 0xFF);

    // Depth (1 byte)
    data.push_back(depth_);

    // Parent fingerprint (4 bytes)
    data.push_back((parentFingerprint_ >> 24) & 0xFF);
    data.push_back((parentFingerprint_ >> 16) & 0xFF);
    data.push_back((parentFingerprint_ >> 8) & 0xFF);
    data.push_back(parentFingerprint_ & 0xFF);

    // Child number (4 bytes)
    data.push_back((childNumber_ >> 24) & 0xFF);
    data.push_back((childNumber_ >> 16) & 0xFF);
    data.push_back((childNumber_ >> 8) & 0xFF);
    data.push_back(childNumber_ & 0xFF);

    // Chain code (32 bytes)
    data.insert(data.end(), chainCode_.begin(), chainCode_.end());

    // Public key (33 bytes: compressed)
    Bytes pubKey = getPublicKey()->getEncoded();
    data.insert(data.end(), pubKey.begin(), pubKey.end());

    // Base58Check encode
    return Base58::encode(data);
} // namespace neocpp
SharedPtr<Bip32ECKeyPair> Bip32ECKeyPair::fromExtendedPrivateKey(const std::string& xprv) {
    Bytes data = Base58::decode(xprv);

    if (data.size() != 78) {
        throw IllegalArgumentException("Invalid extended private key length");
    }

    // Parse components
    uint32_t version = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
    if (version != MAINNET_PRIVATE) {
        throw IllegalArgumentException("Invalid extended private key version");
    }

    uint32_t depth = data[4];
    uint32_t parentFingerprint = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];
    uint32_t childNumber = (data[9] << 24) | (data[10] << 16) | (data[11] << 8) | data[12];

    Bytes chainCode(data.begin() + 13, data.begin() + 45);

    if (data[45] != 0x00) {
        throw IllegalArgumentException("Invalid private key prefix");
    }

    Bytes privateKeyBytes(data.begin() + 46, data.begin() + 78);
    auto privateKey = std::make_shared<ECPrivateKey>(privateKeyBytes);

    return std::make_shared<Bip32ECKeyPair>(privateKey, chainCode, depth,
                                           parentFingerprint, childNumber);
} // namespace neocpp
} // namespace neocpp
