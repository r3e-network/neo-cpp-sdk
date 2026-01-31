#pragma once

#include <string>
#include <vector>
#include <array>
#include <random>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// BIP39 mnemonic phrase generator and validator
class Bip39 {
public:
    /// Mnemonic strength levels
    enum class Strength {
        ENTROPY_128 = 128,  // 12 words
        ENTROPY_160 = 160,  // 15 words
        ENTROPY_192 = 192,  // 18 words
        ENTROPY_224 = 224,  // 21 words
        ENTROPY_256 = 256   // 24 words
    };

    /// Language options
    enum class Language {
        ENGLISH = 0,
        JAPANESE = 1,
        KOREAN = 2,
        SPANISH = 3,
        CHINESE_SIMPLIFIED = 4,
        CHINESE_TRADITIONAL = 5,
        FRENCH = 6,
        ITALIAN = 7,
        CZECH = 8
    };

    /// Generate a new mnemonic phrase
    /// @param strength The entropy strength
    /// @param language The word list language
    /// @return The mnemonic phrase
    static std::string generateMnemonic(Strength strength = Strength::ENTROPY_128,
                                        Language language = Language::ENGLISH);

    /// Generate a new mnemonic phrase from entropy
    /// @param entropy The entropy bytes
    /// @param language The word list language
    /// @return The mnemonic phrase
    static std::string generateMnemonic(const Bytes& entropy,
                                        Language language = Language::ENGLISH);

    /// Validate a mnemonic phrase
    /// @param mnemonic The mnemonic phrase
    /// @param language The word list language
    /// @return True if valid
    static bool validateMnemonic(const std::string& mnemonic,
                                 Language language = Language::ENGLISH);

    /// Convert mnemonic to seed
    /// @param mnemonic The mnemonic phrase
    /// @param passphrase Optional passphrase
    /// @return The seed bytes (64 bytes)
    static Bytes mnemonicToSeed(const std::string& mnemonic,
                                const std::string& passphrase = "");

    /// Convert mnemonic to entropy
    /// @param mnemonic The mnemonic phrase
    /// @param language The word list language
    /// @return The entropy bytes
    static Bytes mnemonicToEntropy(const std::string& mnemonic,
                                   Language language = Language::ENGLISH);

    /// Get word list for a language
    /// @param language The language
    /// @return The word list
    static const std::vector<std::string>& getWordList(Language language = Language::ENGLISH);

    /// Split mnemonic into words
    /// @param mnemonic The mnemonic phrase
    /// @return The words
    static std::vector<std::string> splitMnemonic(const std::string& mnemonic);

    /// Join words into mnemonic
    /// @param words The words
    /// @return The mnemonic phrase
    static std::string joinWords(const std::vector<std::string>& words);

private:
    /// Generate random entropy
    static Bytes generateEntropy(Strength strength);

    /// Calculate checksum for entropy
    static uint8_t calculateChecksum(const Bytes& entropy);

    /// Load word list for language
    static void loadWordList(Language language);

    /// Word lists cache
    static std::vector<std::vector<std::string>> wordLists_;

    /// English word list (2048 words)
    static const std::array<const char*, 2048> ENGLISH_WORDS;
};

} // namespace neocpp
