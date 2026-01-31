#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// TransactionAttribute response type
class TransactionAttribute {
private:
    nlohmann::json data_;

public:
    /// Constructor
    TransactionAttribute() = default;

    /// Constructor from JSON
    explicit TransactionAttribute(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<TransactionAttribute> fromJson(const nlohmann::json& json) {
        return std::make_shared<TransactionAttribute>(json);
    }
};

} // namespace neocpp
