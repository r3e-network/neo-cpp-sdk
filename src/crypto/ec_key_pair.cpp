#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/crypto/wif.hpp"
#include "neocpp/utils/hex.hpp"
#include "neocpp/utils/address.hpp"
#include "neocpp/script/script_builder.hpp"
#include "neocpp/exceptions.hpp"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/obj_mac.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <cstring>
#include <algorithm>

namespace neocpp {

// ECPrivateKey implementation

ECPrivateKey ECPrivateKey::generate() {
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        throw CryptoException("Failed to create EC_KEY");
    }

    if (EC_KEY_generate_key(eckey) != 1) {
        EC_KEY_free(eckey);
        throw CryptoException("Failed to generate EC key");
    }

    const BIGNUM* priv_bn = EC_KEY_get0_private_key(eckey);
    std::array<uint8_t, 32> key;
    key.fill(0);
    BN_bn2binpad(priv_bn, key.data(), 32);

    EC_KEY_free(eckey);
    return ECPrivateKey(key);
} // namespace neocpp
ECPrivateKey::ECPrivateKey(const Bytes& bytes) {
    if (bytes.size() != NeoConstants::PRIVATE_KEY_SIZE) {
        throw IllegalArgumentException("Private key must be 32 bytes");
    }
    std::copy(bytes.begin(), bytes.end(), key_.begin());

    // Verify the private key is valid using OpenSSL
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        throw CryptoException("Failed to create EC_KEY");
    }

    BIGNUM* priv_bn = BN_bin2bn(key_.data(), 32, nullptr);
    if (!priv_bn || EC_KEY_set_private_key(eckey, priv_bn) != 1) {
        if (priv_bn) BN_free(priv_bn);
        EC_KEY_free(eckey);
        throw IllegalArgumentException("Invalid private key");
    }

    BN_free(priv_bn);
    EC_KEY_free(eckey);
} // namespace neocpp
ECPrivateKey::ECPrivateKey(const std::array<uint8_t, NeoConstants::PRIVATE_KEY_SIZE>& key)
    : key_(key) {
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        throw CryptoException("Failed to create EC_KEY");
    }

    BIGNUM* priv_bn = BN_bin2bn(key_.data(), 32, nullptr);
    if (!priv_bn || EC_KEY_set_private_key(eckey, priv_bn) != 1) {
        if (priv_bn) BN_free(priv_bn);
        EC_KEY_free(eckey);
        throw IllegalArgumentException("Invalid private key");
    }

    BN_free(priv_bn);
    EC_KEY_free(eckey);
} // namespace neocpp
ECPrivateKey::ECPrivateKey(const std::string& hex)
    : ECPrivateKey(ByteUtils::fromHex(hex)) {
} // namespace neocpp
Bytes ECPrivateKey::getBytes() const {
    return Bytes(key_.begin(), key_.end());
} // namespace neocpp
std::string ECPrivateKey::toHex() const {
    return ByteUtils::toHex(getBytes(), false);
} // namespace neocpp
SharedPtr<ECPublicKey> ECPrivateKey::getPublicKey() const {
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        throw CryptoException("Failed to create EC_KEY");
    }

    BIGNUM* priv_bn = BN_bin2bn(key_.data(), 32, nullptr);
    if (!priv_bn || EC_KEY_set_private_key(eckey, priv_bn) != 1) {
        if (priv_bn) BN_free(priv_bn);
        EC_KEY_free(eckey);
        throw CryptoException("Failed to set private key");
    }
    BN_free(priv_bn);

    // Generate public key from private key
    const EC_GROUP* group = EC_KEY_get0_group(eckey);
    EC_POINT* pub_point = EC_POINT_new(group);
    BN_CTX* ctx = BN_CTX_new();

    if (!EC_POINT_mul(group, pub_point, EC_KEY_get0_private_key(eckey), nullptr, nullptr, ctx)) {
        BN_CTX_free(ctx);
        EC_POINT_free(pub_point);
        EC_KEY_free(eckey);
        throw CryptoException("Failed to generate public key");
    }

    EC_KEY_set_public_key(eckey, pub_point);

    // Get compressed public key (33 bytes)
    size_t len = 33;
    Bytes encoded(len);
    EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_COMPRESSED, encoded.data(), len, ctx);

    BN_CTX_free(ctx);
    EC_POINT_free(pub_point);
    EC_KEY_free(eckey);
    return std::make_shared<ECPublicKey>(encoded);
} // namespace neocpp
SharedPtr<ECDSASignature> ECPrivateKey::sign(const Bytes& message) const {
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        throw CryptoException("Failed to create EC_KEY");
    }

    BIGNUM* priv_bn = BN_bin2bn(key_.data(), 32, nullptr);
    if (!priv_bn || EC_KEY_set_private_key(eckey, priv_bn) != 1) {
        if (priv_bn) BN_free(priv_bn);
        EC_KEY_free(eckey);
        throw CryptoException("Failed to set private key");
    }
    BN_free(priv_bn);

    // Hash the message
    Bytes hash = HashUtils::sha256(message);

    // Sign the hash
    ECDSA_SIG* sig = ECDSA_do_sign(hash.data(), hash.size(), eckey);
    if (!sig) {
        EC_KEY_free(eckey);
        throw SignException("Failed to sign message");
    }

    // Get r and s components
    const BIGNUM* r;
    const BIGNUM* s;
    ECDSA_SIG_get0(sig, &r, &s);

    // Convert to compact format (64 bytes: 32 for r, 32 for s)
    Bytes signature(64);
    BN_bn2binpad(r, signature.data(), 32);
    BN_bn2binpad(s, signature.data() + 32, 32);

    ECDSA_SIG_free(sig);
    EC_KEY_free(eckey);
    return std::make_shared<ECDSASignature>(signature);
} // namespace neocpp
ECPrivateKey::~ECPrivateKey() {
    // Securely wipe the private key from memory using OpenSSL's secure zeroization
    OPENSSL_cleanse(key_.data(), key_.size());
} // namespace neocpp
// ECPublicKey implementation

ECPublicKey::ECPublicKey(const ECPoint& point) : point_(point) {
} // namespace neocpp
ECPublicKey::ECPublicKey(const Bytes& encoded) : point_(encoded) {
} // namespace neocpp
ECPublicKey::ECPublicKey(const std::string& hex) : point_(hex) {
} // namespace neocpp
Bytes ECPublicKey::getEncoded() const {
    return point_.getEncodedCompressed();
} // namespace neocpp
Bytes ECPublicKey::getEncodedUncompressed() const {
    return point_.getEncodedUncompressed();
} // namespace neocpp
std::string ECPublicKey::toHex() const {
    return point_.toHex();
} // namespace neocpp
bool ECPublicKey::verify(const Bytes& message, const SharedPtr<ECDSASignature>& signature) const {
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!eckey) {
        return false;
    }

    // Set public key
    const EC_GROUP* group = EC_KEY_get0_group(eckey);
    EC_POINT* pub_point = EC_POINT_new(group);
    Bytes encoded = getEncoded();

    if (!EC_POINT_oct2point(group, pub_point, encoded.data(), encoded.size(), nullptr)) {
        EC_POINT_free(pub_point);
        EC_KEY_free(eckey);
        return false;
    }

    EC_KEY_set_public_key(eckey, pub_point);
    EC_POINT_free(pub_point);

    // Parse signature
    Bytes sigBytes = signature->getBytes();
    ECDSA_SIG* sig = ECDSA_SIG_new();
    if (!sig) {
        EC_KEY_free(eckey);
        return false;
    }

    BIGNUM* r = BN_bin2bn(sigBytes.data(), 32, nullptr);
    BIGNUM* s = BN_bin2bn(sigBytes.data() + 32, 32, nullptr);
    ECDSA_SIG_set0(sig, r, s);

    // Hash the message
    Bytes hash = HashUtils::sha256(message);

    // Verify
    int valid = ECDSA_do_verify(hash.data(), hash.size(), sig, eckey);

    ECDSA_SIG_free(sig);
    EC_KEY_free(eckey);
    return valid == 1;
} // namespace neocpp
Bytes ECPublicKey::getScriptHash() const {
    Bytes script = ScriptBuilder::buildVerificationScript(std::make_shared<ECPublicKey>(*this));
    // Hash the script and return as little-endian
    Bytes hash = HashUtils::sha256ThenRipemd160(script);
    std::reverse(hash.begin(), hash.end());
    return hash;
} // namespace neocpp
std::string ECPublicKey::getAddress() const {
    return AddressUtils::scriptHashToAddress(getScriptHash());
}

SharedPtr<ECPublicKey> ECPublicKey::from(const Bytes& data) {
    return std::make_shared<ECPublicKey>(data);
}

// Comparison operators are already defined in the header

// ECKeyPair implementation

ECKeyPair ECKeyPair::generate() {
    return ECKeyPair(std::make_shared<ECPrivateKey>(ECPrivateKey::generate()));
} // namespace neocpp
ECKeyPair::ECKeyPair(const SharedPtr<ECPrivateKey>& privateKey)
    : privateKey_(privateKey), publicKey_(privateKey->getPublicKey()) {
} // namespace neocpp
ECKeyPair::ECKeyPair(const Bytes& privateKeyBytes)
    : ECKeyPair(std::make_shared<ECPrivateKey>(privateKeyBytes)) {
} // namespace neocpp
ECKeyPair ECKeyPair::fromWIF(const std::string& wif) {
    Bytes privateKeyBytes = WIF::decode(wif);
    return ECKeyPair(privateKeyBytes);
} // namespace neocpp
std::string ECKeyPair::exportAsWIF() const {
    return WIF::encode(privateKey_->getBytes());
} // namespace neocpp
SharedPtr<ECDSASignature> ECKeyPair::sign(const Bytes& message) const {
    return privateKey_->sign(message);
} // namespace neocpp
Bytes ECKeyPair::getScriptHash() const {
    return publicKey_->getScriptHash();
} // namespace neocpp
std::string ECKeyPair::getAddress() const {
    return publicKey_->getAddress();
} // namespace neocpp
} // namespace neocpp
