#pragma once

#include "neocpp/wallet/wallet.hpp"
#include <nlohmann/json.hpp>

namespace neocpp {

/// NEP-6 standard wallet implementation
class Nep6Wallet : public Wallet {
private:
    nlohmann::json extra_;
    std::vector<nlohmann::json> tokens_;

public:
    /// Constructor
    /// @param name The wallet name
    /// @param version The wallet version
    explicit Nep6Wallet(const std::string& name = "NeoCpp Wallet", const std::string& version = "1.0");

    /// Destructor
    ~Nep6Wallet() = default;

    /// Get extra data
    const nlohmann::json& getExtra() const { return extra_; }

    /// Set extra data
    void setExtra(const nlohmann::json& extra) { extra_ = extra; }

    /// Get tokens
    const std::vector<nlohmann::json>& getTokens() const { return tokens_; }

    /// Add a token
    void addToken(const nlohmann::json& token) { tokens_.push_back(token); }

    /// Save wallet to NEP-6 JSON format
    /// @param filepath The file path to save to
    /// @param password Optional password for encryption
    void save(const std::string& filepath, const std::string& password = "") const override;

    /// Convert wallet to NEP-6 JSON
    /// @return The NEP-6 JSON representation
    nlohmann::json toJson() const;

    /// Load wallet from NEP-6 JSON
    /// @param json The NEP-6 JSON
    /// @param password Optional password for decryption
    /// @return The loaded wallet
    static SharedPtr<Nep6Wallet> fromJson(const nlohmann::json& json, const std::string& password = "");

    /// Load wallet from file
    /// @param filepath The file path to load from
    /// @param password Optional password for decryption
    /// @return The loaded wallet
    static SharedPtr<Nep6Wallet> load(const std::string& filepath, const std::string& password = "");
};

} // namespace neocpp
