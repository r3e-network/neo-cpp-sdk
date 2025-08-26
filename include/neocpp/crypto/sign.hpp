#pragma once

#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

// Forward declarations
class ECKeyPair;
class ECPrivateKey;
class ECPublicKey;
class ECDSASignature;
class ECPoint;
struct EC_GROUP;
struct BIGNUM;

/// Signing utilities for Neo
class Sign {
public:
    static const int LOWER_REAL_V = 27;
    
    /// Sign a message with a private key
    /// @param message The message to sign
    /// @param privateKey The private key
    /// @return The signature
    static SharedPtr<ECDSASignature> signMessage(const Bytes& message, const SharedPtr<ECPrivateKey>& privateKey);
    
    /// Sign a message with a key pair
    /// @param message The message to sign
    /// @param keyPair The key pair
    /// @return The signature
    static SharedPtr<ECDSASignature> signMessage(const Bytes& message, const SharedPtr<ECKeyPair>& keyPair);
    
    /// Verify a signature
    /// @param message The original message
    /// @param signature The signature to verify
    /// @param publicKey The public key
    /// @return True if signature is valid
    static bool verifySignature(const Bytes& message, const SharedPtr<ECDSASignature>& signature, 
                                const SharedPtr<ECPublicKey>& publicKey);
    
    /// Sign a hash directly (no additional hashing)
    /// @param hash The hash to sign (32 bytes)
    /// @param privateKey The private key
    /// @return The signature
    static SharedPtr<ECDSASignature> signHash(const Bytes& hash, const SharedPtr<ECPrivateKey>& privateKey);
    
    /// Create a signature for Neo transaction
    /// @param txHash The transaction hash
    /// @param privateKey The private key
    /// @return The signature bytes
    static Bytes signTransaction(const Bytes& txHash, const SharedPtr<ECPrivateKey>& privateKey);
    
    /// Recovers the public key from a signature and message
    /// @param recId Recovery ID (0-3)
    /// @param sig The signature
    /// @param message The message hash
    /// @return The recovered public key or nullptr if recovery failed
    static SharedPtr<ECPublicKey> recoverFromSignature(int recId, const SharedPtr<ECDSASignature>& sig, const Bytes& message);
    
    /// Get public key from private key
    /// @param privKey The private key
    /// @return The corresponding public key
    static SharedPtr<ECPublicKey> publicKeyFromPrivateKey(const SharedPtr<ECPrivateKey>& privKey);
    
    /// Recover the signer's script hash from a signature
    /// @param message The message that was signed
    /// @param signatureData The signature data (65 bytes: r, s, v)
    /// @return The script hash of the signer
    static Hash160 recoverSigningScriptHash(const Bytes& message, const Bytes& signatureData);
    
    /// Get the real V value for recovery
    /// @param v The original V value
    /// @return The normalized V value
    static uint8_t getRealV(uint8_t v);
    
private:
    /// Decompress a compressed public key
    /// @param group The EC group
    /// @param x The x coordinate
    /// @param yBit The y bit (0 or 1)
    /// @return The decompressed EC point
    static EC_POINT* decompressKey(const EC_GROUP* group, const BIGNUM* x, int yBit);
};

} // namespace neocpp