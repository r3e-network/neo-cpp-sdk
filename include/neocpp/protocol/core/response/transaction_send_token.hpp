#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// TransactionSendToken response type
class TransactionSendToken {
private:
    nlohmann::json data_;

public:
    /// Constructor
    TransactionSendToken() = default;

    /// Constructor from JSON
    explicit TransactionSendToken(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<TransactionSendToken> fromJson(const nlohmann::json& json) {
        return std::make_shared<TransactionSendToken>(json);
    }
};

} // namespace neocpp
