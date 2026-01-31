#pragma once

#include <string>
#include "neocpp/types/types.hpp"
#include "neocpp/crypto/scrypt_params.hpp"

namespace neocpp {

// Forward declaration
class ECKeyPair;

/// NEP-2 (Neo Enhancement Proposal 2) encryption/decryption for private keys
class NEP2 {
public:
    /// Encrypt a private key using NEP-2
    /// @param privateKey The private key to encrypt
    /// @param password The password to use for encryption
    /// @param params The scrypt parameters
    /// @return The NEP-2 encrypted string
    static std::string encrypt(const Bytes& privateKey, const std::string& password, const ScryptParams& params = ScryptParams::getDefault());

    /// Encrypt a key pair using NEP-2
    /// @param keyPair The key pair to encrypt
    /// @param password The password to use for encryption
    /// @param params The scrypt parameters
    /// @return The NEP-2 encrypted string
    static std::string encrypt(const ECKeyPair& keyPair, const std::string& password, const ScryptParams& params = ScryptParams::getDefault());

    /// Decrypt a NEP-2 encrypted private key
    /// @param nep2 The NEP-2 encrypted string
    /// @param password The password to use for decryption
    /// @param params The scrypt parameters
    /// @return The decrypted private key bytes
    static Bytes decrypt(const std::string& nep2, const std::string& password, const ScryptParams& params = ScryptParams::getDefault());

    /// Decrypt a NEP-2 encrypted private key to a key pair
    /// @param nep2 The NEP-2 encrypted string
    /// @param password The password to use for decryption
    /// @param params The scrypt parameters
    /// @return The decrypted key pair
    static ECKeyPair decryptToKeyPair(const std::string& nep2, const std::string& password, const ScryptParams& params = ScryptParams::getDefault());

    /// Validate a NEP-2 string format
    /// @param nep2 The NEP-2 string to validate
    /// @return True if valid format, false otherwise
    static bool isValid(const std::string& nep2);

    /// Get the address from a NEP-2 encrypted key (without decrypting)
    /// @param nep2 The NEP-2 encrypted string
    /// @return The address
    static std::string getAddress(const std::string& nep2);

private:
    static constexpr uint8_t NEP2_PREFIX_1 = 0x01;
    static constexpr uint8_t NEP2_PREFIX_2 = 0x42;
    static constexpr uint8_t NEP2_FLAG = 0xE0;
    static constexpr size_t NEP2_ENCRYPTED_SIZE = 39;
};

} // namespace neocpp
