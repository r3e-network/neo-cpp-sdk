#include "neocpp/crypto/sign.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/ec_point.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/neo_constants.hpp"
#include "neocpp/crypto/hash.hpp"
#include "neocpp/utils/hex.hpp"
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <cstring>

namespace neocpp {

SharedPtr<ECDSASignature> Sign::signMessage(const Bytes& message, const SharedPtr<ECPrivateKey>& privateKey) {
    return privateKey->sign(message);
}

SharedPtr<ECDSASignature> Sign::signMessage(const Bytes& message, const SharedPtr<ECKeyPair>& keyPair) {
    return signMessage(message, keyPair->getPrivateKey());
}

bool Sign::verifySignature(const Bytes& message, const SharedPtr<ECDSASignature>& signature, const SharedPtr<ECPublicKey>& publicKey) {
    // ECPublicKey::verify() hashes the message internally, so we pass message directly
    return publicKey->verify(message, signature);
}

SharedPtr<ECDSASignature> Sign::signHash(const Bytes& hash, const SharedPtr<ECPrivateKey>& privateKey) {
    // For now, delegate to signMessage which will hash again
    // TODO: Implement proper signHash that doesn't double-hash
    return privateKey->sign(hash);
}

Bytes Sign::signTransaction(const Bytes& txHash, const SharedPtr<ECPrivateKey>& privateKey) {
    auto signature = signHash(txHash, privateKey);
    return signature->getBytes();
}

SharedPtr<ECPublicKey> Sign::recoverFromSignature(int recId, const SharedPtr<ECDSASignature>& sig, const Bytes& message) {
    if (recId < 0 || recId > 3) {
        throw IllegalArgumentException("Recovery ID must be between 0 and 3");
    }

    if (!sig) {
        throw IllegalArgumentException("Signature cannot be null");
    }

    if (message.empty()) {
        throw IllegalArgumentException("Message cannot be empty");
    }

    // Get the curve (secp256r1 for Neo N3)
    EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    if (!group) {
        throw CryptoException("Failed to create EC_GROUP");
    }

    // Get signature components
    Bytes rBytes = sig->getR();
    Bytes sBytes = sig->getS();

    // Convert to BIGNUMs
    BIGNUM* r = BN_bin2bn(rBytes.data(), static_cast<int>(rBytes.size()), nullptr);
    BIGNUM* s = BN_bin2bn(sBytes.data(), static_cast<int>(sBytes.size()), nullptr);

    if (!r || !s) {
        BN_free(r);
        BN_free(s);
        EC_GROUP_free(group);
        throw CryptoException("Failed to convert signature components");
    }

    // Get curve order
    BIGNUM* order = BN_new();
    BIGNUM* x = BN_new();
    BIGNUM* fieldPrime = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    if (!order || !x || !fieldPrime || !ctx) {
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        if (ctx) BN_CTX_free(ctx);
        throw CryptoException("Failed to allocate BIGNUMs");
    }

    EC_GROUP_get_order(group, order, ctx);
    EC_GROUP_get_curve(group, fieldPrime, nullptr, nullptr, ctx);

    // Calculate x = r + (recId / 2) * n
    BIGNUM* recDiv2 = BN_new();
    BIGNUM* recDiv2MulN = BN_new();
    BN_set_word(recDiv2, recId / 2);
    BN_mul(recDiv2MulN, recDiv2, order, ctx);
    BN_add(x, r, recDiv2MulN);

    BN_free(recDiv2);
    BN_free(recDiv2MulN);

    // Check if x is within field prime
    if (BN_cmp(x, fieldPrime) >= 0) {
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        return nullptr;  // Invalid recovery ID for this signature
    }

    // Decompress the point
    EC_POINT* R = EC_POINT_new(group);
    if (!R) {
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        throw CryptoException("Failed to create EC_POINT");
    }

    // Set compressed coordinates (recId & 1 gives y parity)
    if (!EC_POINT_set_compressed_coordinates(group, R, x, recId & 1, ctx)) {
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        return nullptr;
    }

    // Verify R is on curve
    if (!EC_POINT_is_on_curve(group, R, ctx)) {
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        return nullptr;
    }

    // Check n*R == infinity
    EC_POINT* nR = EC_POINT_new(group);
    if (!nR) {
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        throw CryptoException("Failed to create EC_POINT");
    }

    EC_POINT_mul(group, nR, nullptr, R, order, ctx);
    if (!EC_POINT_is_at_infinity(group, nR)) {
        EC_POINT_free(nR);
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        return nullptr;
    }

    EC_POINT_free(nR);

    // Calculate e (message hash as integer)
    BIGNUM* e = BN_bin2bn(message.data(), static_cast<int>(message.size()), nullptr);
    if (!e) {
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        throw CryptoException("Failed to convert message to BIGNUM");
    }

    BN_mod(e, e, order, ctx);

    // Calculate r^-1 mod n
    BIGNUM* rInv = BN_mod_inverse(nullptr, r, order, ctx);
    if (!rInv) {
        BN_free(e);
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        throw CryptoException("Failed to compute r inverse");
    }

    // Calculate Q = r^-1 * (s*R - e*G)
    // This is: Q = (s*r^-1)*R + (-e*r^-1)*G

    // srInv = s * r^-1 mod n
    BIGNUM* srInv = BN_new();
    BN_mod_mul(srInv, s, rInv, order, ctx);

    // -e*r^-1 mod n
    BIGNUM* eNeg = BN_new();
    BN_sub(eNeg, order, e);
    BN_mod_mul(eNeg, eNeg, rInv, order, ctx);

    EC_POINT* Q = EC_POINT_new(group);
    if (!Q) {
        BN_free(srInv);
        BN_free(eNeg);
        BN_free(rInv);
        BN_free(e);
        EC_POINT_free(R);
        BN_free(r);
        BN_free(s);
        BN_free(order);
        BN_free(x);
        BN_free(fieldPrime);
        EC_GROUP_free(group);
        BN_CTX_free(ctx);
        throw CryptoException("Failed to create EC_POINT for Q");
    }

    // Q = srInv * R + eNeg * G (G is implicitly used when multiplying by order)
    EC_POINT_mul(group, Q, eNeg, R, srInv, ctx);

    // Cleanup intermediate values
    BN_free(srInv);
    BN_free(eNeg);
    BN_free(rInv);
    BN_free(e);
    EC_POINT_free(R);
    BN_free(r);
    BN_free(s);
    BN_free(order);
    BN_free(x);
    BN_free(fieldPrime);

    // Get the public key bytes
    size_t pubKeyLen = 65;  // Uncompressed
    Bytes pubKeyBytes(pubKeyLen);
    EC_POINT_point2oct(group, Q, POINT_CONVERSION_UNCOMPRESSED, pubKeyBytes.data(), pubKeyLen, ctx);

    EC_POINT_free(Q);
    EC_GROUP_free(group);
    BN_CTX_free(ctx);

    // Create ECPublicKey from the recovered point
    try {
        return std::make_shared<ECPublicKey>(pubKeyBytes);
    } catch (const std::exception& ex) {
        return nullptr;
    }
}

SharedPtr<ECPublicKey> Sign::publicKeyFromPrivateKey(const SharedPtr<ECPrivateKey>& privKey) {
    return privKey->getPublicKey();
}

Hash160 Sign::recoverSigningScriptHash(const Bytes& message, const Bytes& signatureData) {
    if (signatureData.size() != 65) {
        throw IllegalArgumentException("Signature data must be 65 bytes (r + s + v)");
    }

    // Extract components from signature data
    Bytes r(signatureData.begin(), signatureData.begin() + 32);
    Bytes s(signatureData.begin() + 32, signatureData.begin() + 64);
    uint8_t v = signatureData[64];

    // Get recovery ID
    uint8_t realV = getRealV(v);
    int recId = realV - LOWER_REAL_V;

    if (recId < 0 || recId > 3) {
        throw CryptoException("Invalid recovery ID in signature");
    }

    // Create signature object
    auto sig = std::make_shared<ECDSASignature>(r, s);

    // Hash the message (it should already be a hash, but we ensure it)
    Bytes messageHash;
    if (message.size() == 32) {
        messageHash = message;
    } else {
        messageHash = HashUtils::sha256(message);
    }

    // Recover the public key
    auto publicKey = recoverFromSignature(recId, sig, messageHash);

    if (!publicKey) {
        throw CryptoException("Failed to recover public key from signature");
    }

    // Get the script hash from the public key
    return Hash160::fromPublicKey(publicKey->getEncoded());
}

uint8_t Sign::getRealV(uint8_t v) {
    if (v == LOWER_REAL_V || v == LOWER_REAL_V + 1) {
        return v;
    }
    uint8_t realV = LOWER_REAL_V;
    int inc = (v % 2 == 0) ? 1 : 0;
    return realV + inc;
}

EC_POINT* Sign::decompressKey(const EC_GROUP* group, const BIGNUM* x, int yBit) {
    if (!group || !x) {
        return nullptr;
    }

    EC_POINT* point = EC_POINT_new(group);
    if (!point) {
        return nullptr;
    }

    BN_CTX* ctx = BN_CTX_new();
    if (!ctx) {
        EC_POINT_free(point);
        return nullptr;
    }

    // Set compressed coordinates
    if (!EC_POINT_set_compressed_coordinates(group, point, x, yBit, ctx)) {
        EC_POINT_free(point);
        BN_CTX_free(ctx);
        return nullptr;
    }

    BN_CTX_free(ctx);
    return point;
}

} // namespace neocpp
