#pragma once

#include <string>
#include <vector>
#include <memory>
#include "neocpp/crypto/ec_key_pair.hpp"

namespace neocpp {

/// BIP32 hierarchical deterministic key pair
class Bip32ECKeyPair : public ECKeyPair {
private:
    Bytes chainCode_;
    uint32_t depth_;
    uint32_t parentFingerprint_;
    uint32_t childNumber_;

public:
    /// Generate master key from seed
    /// @param seed The seed bytes
    /// @return The master key pair
    static SharedPtr<Bip32ECKeyPair> fromSeed(const Bytes& seed);

    /// Generate master key from mnemonic
    /// @param mnemonic The mnemonic phrase
    /// @param passphrase Optional passphrase
    /// @return The master key pair
    static SharedPtr<Bip32ECKeyPair> fromMnemonic(const std::string& mnemonic, const std::string& passphrase = "");

    /// Derive child key
    /// @param index The child index
    /// @param hardened Whether to use hardened derivation
    /// @return The child key pair
    SharedPtr<Bip32ECKeyPair> deriveChild(uint32_t index, bool hardened = false);

    /// Derive key from path
    /// @param path The derivation path (e.g., "m/44'/888'/0'/0/0")
    /// @return The derived key pair
    SharedPtr<Bip32ECKeyPair> derivePath(const std::string& path);

    /// Get the chain code
    /// @return The chain code
    const Bytes& getChainCode() const { return chainCode_; }

    /// Get the depth
    /// @return The depth in derivation path
    [[nodiscard]] uint32_t getDepth() const { return depth_; }

    /// Get the parent fingerprint
    /// @return The parent fingerprint
    [[nodiscard]] uint32_t getParentFingerprint() const { return parentFingerprint_; }

    /// Get the child number
    /// @return The child number
    [[nodiscard]] uint32_t getChildNumber() const { return childNumber_; }

    /// Export as extended private key
    /// @return The extended private key string
    std::string toExtendedPrivateKey() const;

    /// Export as extended public key
    /// @return The extended public key string
    std::string toExtendedPublicKey() const;

    /// Import from extended private key
    /// @param xprv The extended private key string
    /// @return The key pair
    static SharedPtr<Bip32ECKeyPair> fromExtendedPrivateKey(const std::string& xprv);

    /// Constructor
    Bip32ECKeyPair(const SharedPtr<ECPrivateKey>& privateKey, const Bytes& chainCode,
                   uint32_t depth, uint32_t parentFingerprint, uint32_t childNumber);
};

} // namespace neocpp
