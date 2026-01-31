#pragma once

#include <cstdint>
#include <cstddef>

namespace neocpp {

class NeoConstants {
public:
    // Accounts, Addresses, Keys

    /// The maximum number of public keys that can take part in a multi-signature address.
    static constexpr int MAX_PUBLIC_KEYS_PER_MULTISIG_ACCOUNT = 1024;

    /// The byte size of a Hash160 hash.
    static constexpr int HASH160_SIZE = 20;

    /// The byte size of a Hash256 hash.
    static constexpr int HASH256_SIZE = 32;

    /// Size of a private key in bytes.
    static constexpr int PRIVATE_KEY_SIZE = 32;

    /// Size of a compressed public key in bytes.
    static constexpr int PUBLIC_KEY_SIZE_COMPRESSED = 33;

    /// Size of a signature in bytes.
    static constexpr int SIGNATURE_SIZE = 64;

    /// Size of a single signature verification script in bytes.
    static constexpr int VERIFICATION_SCRIPT_SIZE = 40;

    // Transactions & Contracts

    /// The current version used for Neo transaction.
    static constexpr uint8_t CURRENT_TX_VERSION = 0;

    /// The maximum size of a transaction.
    static constexpr int MAX_TRANSACTION_SIZE = 102400;

    /// The maximum number of attributes that a transaction can have.
    static constexpr int MAX_TRANSACTION_ATTRIBUTES = 16;
    
    /// The maximum valid until block increment (transaction lifetime in blocks)
    static constexpr uint32_t MAX_VALID_UNTIL_BLOCK_INCREMENT = 86400000; // ~1 day in milliseconds

    /// The maximum number of contracts or groups a signer scope can contain.
    static constexpr int MAX_SIGNER_SUBITEMS = 16;

    /// The maximum byte length for a valid contract manifest.
    static constexpr int MAX_MANIFEST_SIZE = 0xFFFF;

    /// The default maximum number of iterator items returned in an RPC response.
    static constexpr int MAX_ITERATOR_ITEMS_DEFAULT = 100;

    // Network

    /// Magic number for Neo N3 MainNet
    static constexpr uint32_t NEO_N3_MAINNET_MAGIC = 0x334F454E; // N3MainNet

    /// Magic number for Neo N3 TestNet
    static constexpr uint32_t NEO_N3_TESTNET_MAGIC = 0x3454354E; // N3TestNet

    /// Default RPC port
    static constexpr int DEFAULT_RPC_PORT = 10332;

    // Address prefixes
    static constexpr uint8_t ADDRESS_VERSION = 0x35; // Neo N3 address version (53 in decimal)


    // Hardfork settings for Neo v3.9.1
    static constexpr uint32_t HF_Aspidochelone = 210000;
    static constexpr uint32_t HF_Basilisk = 220000;
    static constexpr uint32_t HF_Cockatrice = 230000;

private:
    NeoConstants() = delete; // Prevent instantiation
};

} // namespace neocpp
