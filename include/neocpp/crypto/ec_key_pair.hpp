#pragma once

#include <string>
#include <memory>
#include <array>
#include "neocpp/types/types.hpp"
#include "neocpp/crypto/ec_point.hpp"
#include "neocpp/neo_constants.hpp"

namespace neocpp {

// Forward declarations
class ECPrivateKey;
class ECPublicKey;
class ECDSASignature;

/// Represents an EC private key
class ECPrivateKey {
private:
    std::array<uint8_t, NeoConstants::PRIVATE_KEY_SIZE> key_;
    
public:
    /// Generate a random private key
    static ECPrivateKey generate();
    
    /// Construct from bytes
    /// @param bytes The private key bytes (32 bytes)
    explicit ECPrivateKey(const Bytes& bytes);
    
    /// Construct from array
    /// @param key The private key array
    explicit ECPrivateKey(const std::array<uint8_t, NeoConstants::PRIVATE_KEY_SIZE>& key);
    
    /// Construct from hex string
    /// @param hex The hex-encoded private key
    explicit ECPrivateKey(const std::string& hex);
    
    /// Get the private key bytes
    /// @return The private key as bytes
    Bytes getBytes() const;
    
    /// Get the private key as hex string
    /// @return The hex-encoded private key
    std::string toHex() const;
    
    /// Derive the public key
    /// @return The corresponding public key
    SharedPtr<ECPublicKey> getPublicKey() const;
    
    /// Sign a message
    /// @param message The message to sign
    /// @return The signature
    SharedPtr<ECDSASignature> sign(const Bytes& message) const;
};

/// Represents an EC public key
class ECPublicKey {
private:
    ECPoint point_;
    
public:
    /// Construct from ECPoint
    /// @param point The EC point
    explicit ECPublicKey(const ECPoint& point);
    
    /// Construct from encoded bytes
    /// @param encoded The encoded public key
    explicit ECPublicKey(const Bytes& encoded);
    
    /// Construct from hex string
    /// @param hex The hex-encoded public key
    explicit ECPublicKey(const std::string& hex);
    
    /// Get the EC point
    /// @return The EC point
    const ECPoint& getPoint() const { return point_; }
    
    /// Get the encoded bytes (compressed)
    /// @return The compressed encoded public key
    Bytes getEncoded() const;
    
    /// Get the encoded bytes (compressed) - alias for getEncoded()
    /// @return The compressed encoded public key
    Bytes getEncodedCompressed() const { return getEncoded(); }
    
    /// Get the encoded bytes (uncompressed)
    /// @return The uncompressed encoded public key
    Bytes getEncodedUncompressed() const;
    
    /// Convert to hex string
    /// @return The hex-encoded public key
    std::string toHex() const;
    
    /// Verify a signature
    /// @param message The message that was signed
    /// @param signature The signature to verify
    /// @return True if signature is valid
    bool verify(const Bytes& message, const SharedPtr<ECDSASignature>& signature) const;
    
    /// Get the script hash for this public key
    /// @return The script hash
    Bytes getScriptHash() const;
    
    /// Get the address for this public key
    
    /// Get the size of the encoded public key
    /// @return The size in bytes
    size_t size() const { return getEncoded().size(); }
    
    /// Convert to byte array
    /// @return The encoded bytes
    Bytes toArray() const { return getEncoded(); }
    
    /// Create from byte array
    /// @param data The encoded bytes
    /// @return The public key
    static SharedPtr<ECPublicKey> from(const Bytes& data);
    
    /// Equality operator
    /// @param other The other public key
    /// @return True if equal
    bool operator==(const ECPublicKey& other) const {
        return point_ == other.point_;
    }
    
    /// Inequality operator
    /// @param other The other public key
    /// @return True if not equal
    bool operator!=(const ECPublicKey& other) const {
        return !(*this == other);
    }
    
    /// Less than operator
    /// @param other The other public key
    /// @return True if this key is less than other
    bool operator<(const ECPublicKey& other) const {
        return point_ < other.point_;
    }
    
    /// Greater than operator
    /// @param other The other public key
    /// @return True if this key is greater than other
    bool operator>(const ECPublicKey& other) const {
        return other < *this;
    }
    
    /// Less than or equal operator
    /// @param other The other public key
    /// @return True if this key is less than or equal to other
    bool operator<=(const ECPublicKey& other) const {
        return !(other < *this);
    }
    
    /// Greater than or equal operator
    /// @param other The other public key
    /// @return True if this key is greater than or equal to other
    bool operator>=(const ECPublicKey& other) const {
        return !(*this < other);
    }
    /// @return The Neo address
    std::string getAddress() const;
};

/// Represents an EC key pair (private and public key)
class ECKeyPair {
private:
    SharedPtr<ECPrivateKey> privateKey_;
    SharedPtr<ECPublicKey> publicKey_;
    
public:
    /// Generate a new random key pair
    static ECKeyPair generate();
    
    /// Construct from private key
    /// @param privateKey The private key
    explicit ECKeyPair(const SharedPtr<ECPrivateKey>& privateKey);
    
    /// Construct from private key bytes
    /// @param privateKeyBytes The private key bytes
    explicit ECKeyPair(const Bytes& privateKeyBytes);
    
    /// Construct from WIF string
    /// @param wif The WIF-encoded private key
    static ECKeyPair fromWIF(const std::string& wif);
    
    /// Get the private key
    /// @return The private key
    const SharedPtr<ECPrivateKey>& getPrivateKey() const { return privateKey_; }
    
    /// Get the public key
    /// @return The public key
    const SharedPtr<ECPublicKey>& getPublicKey() const { return publicKey_; }
    
    /// Export private key as WIF
    /// @return The WIF-encoded private key
    std::string exportAsWIF() const;
    
    /// Sign a message
    /// @param message The message to sign
    /// @return The signature
    SharedPtr<ECDSASignature> sign(const Bytes& message) const;
    
    /// Get the script hash for this key pair
    /// @return The script hash
    Bytes getScriptHash() const;
    
    /// Get the address for this key pair
    /// @return The Neo address
    std::string getAddress() const;
};

} // namespace neocpp