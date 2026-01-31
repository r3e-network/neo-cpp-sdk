#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// TransactionSigner response type
class TransactionSigner {
private:
    nlohmann::json data_;

public:
    /// Constructor
    TransactionSigner() = default;

    /// Constructor from JSON
    explicit TransactionSigner(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<TransactionSigner> fromJson(const nlohmann::json& json) {
        return std::make_shared<TransactionSigner>(json);
    }
};

} // namespace neocpp
