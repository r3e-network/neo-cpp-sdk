#include "neocpp/crypto/sign.hpp"
#include "neocpp/crypto/ec_key_pair.hpp"
#include "neocpp/crypto/ecdsa_signature.hpp"
#include "neocpp/crypto/ec_point.hpp"
#include "neocpp/exceptions.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/types/neo_constants.hpp"
#include "neocpp/utils/hash.hpp"
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <cstring>

namespace neocpp {

const int Sign::LOWER_REAL_V = 27;

SharedPtr<ECDSASignature> Sign::signMessage(const Bytes& message, const SharedPtr<ECPrivateKey>& privateKey) {
    // First hash the message with SHA-256
    Bytes hash = Hash::sha256(message);
    return signHash(hash, privateKey);
}

SharedPtr<ECDSASignature> Sign::signMessage(const Bytes& message, const SharedPtr<ECKeyPair>& keyPair) {
    return signMessage(message, keyPair->getPrivateKey());
}

bool Sign::verifySignature(const Bytes& message, const SharedPtr<ECDSASignature>& signature, const SharedPtr<ECPublicKey>& publicKey) {
    // First hash the message with SHA-256
    Bytes hash = Hash::sha256(message);
    return publicKey->verify(hash, signature);
}

SharedPtr<ECDSASignature> Sign::signHash(const Bytes& hash, const SharedPtr<ECPrivateKey>& privateKey) {
    if (hash.size() != 32) {
        throw IllegalArgumentException("Hash must be 32 bytes");
    }
    
    EC_KEY* ecKey = nullptr;
    try {
        ecKey = privateKey->toOpenSSL();
        unsigned int sigLen = ECDSA_size(ecKey);
        Bytes sigBytes(sigLen);
        
        if (!ECDSA_sign(0, hash.data(), static_cast<int>(hash.size()), sigBytes.data(), &sigLen, ecKey)) {
            throw CryptoException("Failed to sign hash");
        }
        
        // Resize to actual signature length
        sigBytes.resize(sigLen);
        
        // Convert DER signature to compact format
        return std::make_shared<ECDSASignature>(ECDSASignature::fromDER(sigBytes));
    } catch (...) {
        if (ecKey) {
            EC_KEY_free(ecKey);
        }
        throw;
    }
}

Bytes Sign::signTransaction(const Bytes& txHash, const SharedPtr<ECPrivateKey>& privateKey) {
    auto signature = signHash(txHash, privateKey);
    return signature->getBytes();
}

// Additional methods from Swift implementation

/// Recovers the public key from a signature and message
SharedPtr<ECPublicKey> Sign::recoverFromSignature(int recId, const SharedPtr<ECDSASignature>& sig, const Bytes& message) {
    if (recId < 0 || recId > 3) {
        throw IllegalArgumentException("recId must be between 0 and 3");
    }
    
    if (message.empty()) {
        throw IllegalArgumentException("message cannot be empty");
    }
    
    // Get the curve parameters
    const EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1); // secp256r1
    if (!group) {
        throw CryptoException("Failed to create EC_GROUP");
    }
    
    // Get curve order
    BIGNUM* order = BN_new();
    if (!EC_GROUP_get_order(group, order, nullptr)) {
        EC_GROUP_free(group);
        BN_free(order);
        throw CryptoException("Failed to get curve order");
    }
    
    // Compute x = r + i * n
    BIGNUM* i = BN_new();
    BN_set_word(i, recId / 2);
    BIGNUM* i_times_n = BN_new();
    BN_mul(i_times_n, i, order, nullptr);
    
    BIGNUM* r = BN_bin2bn(sig->getR().data(), static_cast<int>(sig->getR().size()), nullptr);
    BIGNUM* x = BN_new();
    BN_add(x, r, i_times_n);
    
    // Get prime
    BIGNUM* prime = BN_new();
    if (!EC_GROUP_get_curve(group, prime, nullptr, nullptr, nullptr)) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        throw CryptoException("Failed to get curve prime");
    }
    
    // Check x <= prime
    if (BN_cmp(x, prime) > 0) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        throw CryptoException("x exceeds prime");
    }
    
    // Decompress the key
    EC_POINT* R = decompressKey(group, x, recId & 1);
    if (!R) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        return nullptr;
    }
    
    // Verify R is on the curve
    if (EC_POINT_is_on_curve(group, R, nullptr) != 1) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        EC_POINT_free(R);
        return nullptr;
    }
    
    // Compute R * n to check if it's infinity
    EC_POINT* R_times_n = EC_POINT_new(group);
    if (!EC_POINT_mul(group, R_times_n, nullptr, R, order, nullptr)) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        EC_POINT_free(R);
        EC_POINT_free(R_times_n);
        throw CryptoException("Failed to compute R * n");
    }
    
    if (!EC_POINT_is_at_infinity(group, R_times_n)) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        EC_POINT_free(R);
        EC_POINT_free(R_times_n);
        return nullptr;
    }
    
    // Compute e = message hash
    BIGNUM* e = BN_bin2bn(message.data(), static_cast<int>(message.size()), nullptr);
    
    // Compute eInv = -e mod n
    BIGNUM* eInv = BN_new();
    BN_mod_sub(eInv, BN_value_one(), e, order, nullptr);
    
    // Compute rInv = r^-1 mod n
    BIGNUM* rInv = BN_mod_inverse(nullptr, r, order, nullptr);
    if (!rInv) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        EC_POINT_free(R);
        EC_POINT_free(R_times_n);
        BN_free(e);
        BN_free(eInv);
        throw CryptoException("Failed to compute r inverse");
    }
    
    // Compute srInv = s * rInv mod n
    BIGNUM* srInv = BN_new();
    BN_mod_mul(srInv, BN_bin2bn(sig->getS().data(), static_cast<int>(sig->getS().size()), nullptr), rInv, order, nullptr);
    
    // Compute eInvrInv = eInv * rInv mod n
    BIGNUM* eInvrInv = BN_new();
    BN_mod_mul(eInvrInv, eInv, rInv, order, nullptr);
    
    // Compute Q = eInvrInv * G + srInv * R
    EC_POINT* G = EC_GROUP_get0_generator(group);
    EC_POINT* Q = EC_POINT_new(group);
    if (!EC_POINT_mul(group, Q, eInvrInv, G, srInv, nullptr)) {
        // Cleanup
        EC_GROUP_free(group);
        BN_free(order);
        BN_free(i);
        BN_free(i_times_n);
        BN_free(r);
        BN_free(x);
        BN_free(prime);
        EC_POINT_free(R);
        EC_POINT_free(R_times_n);
        BN_free(e);
        BN_free(eInv);
        BN_free(rInv);
        BN_free(srInv);
        BN_free(eInvrInv);
        EC_POINT_free(Q);
        throw CryptoException("Failed to compute public key point");
    }
    
    // Cleanup
    EC_GROUP_free(group);
    BN_free(order);
    BN_free(i);
    BN_free(i_times_n);
    BN_free(r);
    BN_free(x);
    BN_free(prime);
    EC_POINT_free(R);
    EC_POINT_free(R_times_n);
    BN_free(e);
    BN_free(eInv);
    BN_free(rInv);
    BN_free(srInv);
    BN_free(eInvrInv);
    
    // Create ECPublicKey from Q
    return std::make_shared<ECPublicKey>(ECPoint(Q, true));
}

/// Decompress a compressed public key
EC_POINT* Sign::decompressKey(const EC_GROUP* group, const BIGNUM* x, int yBit) {
    EC_POINT* point = EC_POINT_new(group);
    if (!point) {
        return nullptr;
    }
    
    // Set x coordinate
    if (!EC_POINT_set_compressed_coordinates(group, point, x, yBit, nullptr)) {
        EC_POINT_free(point);
        return nullptr;
    }
    
    return point;
}

/// Get public key from private key
SharedPtr<ECPublicKey> Sign::publicKeyFromPrivateKey(const SharedPtr<ECPrivateKey>& privKey) {
    return privKey->toPublicKey();
}

/// Recover the signer's script hash from a signature
Hash160 Sign::recoverSigningScriptHash(const Bytes& message, const Bytes& signatureData) {
    // Create a dummy SignatureData-like structure from bytes
    uint8_t v = signatureData[64];
    Bytes r(signatureData.begin(), signatureData.begin() + 32);
    Bytes s(signatureData.begin() + 32, signatureData.begin() + 64);
    
    // Get the real V value
    v = getRealV(v);
    
    // Create signature object
    auto sig = std::make_shared<ECDSASignature>(r, s);
    
    // Hash the message
    Bytes messageHash = Hash::sha256(message);
    
    // Recover the public key
    int recId = v - LOWER_REAL_V;
    auto publicKey = recoverFromSignature(recId, sig, messageHash);
    
    if (!publicKey) {
        throw CryptoException("Failed to recover public key");
    }
    
    // Get the script hash from the public key
    return Hash160::fromPublicKey(publicKey->getEncoded(true));
}

uint8_t Sign::getRealV(uint8_t v) {
    if (v == LOWER_REAL_V || v == LOWER_REAL_V + 1) {
        return v;
    }
    uint8_t realV = LOWER_REAL_V;
    int inc = (v % 2 == 0) ? 1 : 0;
    return realV + inc;
}

} // namespace neocpp