
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/types/types.hpp"
#include "neocpp/exceptions.hpp"
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <cstring>

namespace neocpp {

ECDSASignature::ECDSASignature(const Bytes& signature) : isEmpty_(false) {
    if (signature.empty()) {
        // Empty signature - initialize to zeros and mark as empty
        signature_.fill(0);
        isEmpty_ = true;
        return;
    }
    if (signature.size() != NeoConstants::SIGNATURE_SIZE) {
        throw IllegalArgumentException("Signature must be 64 bytes");
    }
    std::copy(signature.begin(), signature.end(), signature_.begin());
} // namespace neocpp
ECDSASignature::ECDSASignature(const std::array<uint8_t, NeoConstants::SIGNATURE_SIZE>& signature)
    : signature_(signature), isEmpty_(false) {
} // namespace neocpp
ECDSASignature::ECDSASignature(const Bytes& r, const Bytes& s) : isEmpty_(false) {
    if (r.size() != 32 || s.size() != 32) {
        throw IllegalArgumentException("R and S must each be 32 bytes");
    }
    std::copy(r.begin(), r.end(), signature_.begin());
    std::copy(s.begin(), s.end(), signature_.begin() + 32);
} // namespace neocpp
ECDSASignature::ECDSASignature(const std::string& hex)
    : ECDSASignature(ByteUtils::fromHex(hex)) {
} // namespace neocpp
Bytes ECDSASignature::getBytes() const {
    if (isEmpty_) {
        return Bytes();
    }
    return Bytes(signature_.begin(), signature_.end());
} // namespace neocpp
Bytes ECDSASignature::getR() const {
    return Bytes(signature_.begin(), signature_.begin() + 32);
} // namespace neocpp
Bytes ECDSASignature::getS() const {
    return Bytes(signature_.begin() + 32, signature_.end());
} // namespace neocpp
std::string ECDSASignature::toHex() const {
    return ByteUtils::toHex(getBytes(), false);
} // namespace neocpp
Bytes ECDSASignature::toDER() const {
    ECDSA_SIG* sig = ECDSA_SIG_new();
    if (!sig) {
        throw CryptoException("Failed to create ECDSA_SIG");
    }

    // Set r and s values
    BIGNUM* r = BN_bin2bn(signature_.data(), 32, nullptr);
    BIGNUM* s = BN_bin2bn(signature_.data() + 32, 32, nullptr);
    ECDSA_SIG_set0(sig, r, s);

    // Serialize to DER
    unsigned char* der = nullptr;
    int derLen = i2d_ECDSA_SIG(sig, &der);
    if (derLen <= 0) {
        ECDSA_SIG_free(sig);
        throw CryptoException("Failed to serialize signature to DER");
    }

    Bytes result(der, der + derLen);
    OPENSSL_free(der);
    ECDSA_SIG_free(sig);
    return result;
} // namespace neocpp
ECDSASignature ECDSASignature::fromDER(const Bytes& der) {
    const unsigned char* p = der.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &p, der.size());
    if (!sig) {
        throw CryptoException("Failed to parse DER signature");
    }

    // Get r and s values
    const BIGNUM* r;
    const BIGNUM* s;
    ECDSA_SIG_get0(sig, &r, &s);

    // Convert to compact format
    Bytes compact(64);
    BN_bn2binpad(r, compact.data(), 32);
    BN_bn2binpad(s, compact.data() + 32, 32);

    ECDSA_SIG_free(sig);
    return ECDSASignature(compact);
} // namespace neocpp
bool ECDSASignature::isCanonical() const {
    // In canonical form, S must be <= half the curve order
    // For secp256r1 (Neo N3 curve), the half order is:
    // 0x7fffffff800000007fffffffffffffffde737d56d38bcf4279dce5617e3192a8

    Bytes s = getS();

    // Half curve order for secp256r1 (NIST P-256) - Neo N3 curve
    static const uint8_t HALF_CURVE_ORDER[32] = {
        0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00,
        0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xDE, 0x73, 0x7D, 0x56, 0xD3, 0x8B, 0xCF, 0x42,
        0x79, 0xDC, 0xE5, 0x61, 0x7E, 0x31, 0x92, 0xA8
    };

    // Compare S with half curve order
    for (size_t i = 0; i < 32; ++i) {
        if (s[i] < HALF_CURVE_ORDER[i]) {
            return true;  // S is definitely less than half order
        }
        if (s[i] > HALF_CURVE_ORDER[i]) {
            return false; // S is definitely greater than half order
        }
        // If equal, continue to next byte
    }

    // S equals half order, which is still canonical
    return true;
} // namespace neocpp
ECDSASignature ECDSASignature::makeCanonical() const {
    if (isCanonical()) {
        return *this;
    }

    // If S is not canonical, we need to negate it modulo the curve order
    // S' = n - S where n is the curve order

    // Full curve order for secp256r1 (NIST P-256) - Neo N3 curve
    // n = FFFFFFFF 00000000 FFFFFFFF FFFFFFFF BCE6FAAD A7179E84 F3B9CAC2 FC632551
    static const uint8_t CURVE_ORDER[32] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84,
        0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51
    };

    Bytes r = getR();
    Bytes s = getS();

    // Compute n - s using OpenSSL's BIGNUM
    BIGNUM* bn_s = BN_bin2bn(s.data(), 32, nullptr);
    BIGNUM* bn_order = BN_bin2bn(CURVE_ORDER, 32, nullptr);
    BIGNUM* bn_result = BN_new();

    BN_sub(bn_result, bn_order, bn_s); // result = order - s

    // Convert back to bytes
    Bytes new_s(32);
    BN_bn2binpad(bn_result, new_s.data(), 32);

    BN_free(bn_s);
    BN_free(bn_order);
    BN_free(bn_result);

    // Create new signature with canonical S
    return ECDSASignature(r, new_s);
} // namespace neocpp
bool ECDSASignature::operator==(const ECDSASignature& other) const {
    return signature_ == other.signature_;
} // namespace neocpp
bool ECDSASignature::operator!=(const ECDSASignature& other) const {
    return !(*this == other);
} // namespace neocpp
} // namespace neocpp
