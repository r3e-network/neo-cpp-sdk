#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// ContractMethodToken response type
class ContractMethodToken {
private:
    nlohmann::json data_;

public:
    /// Constructor
    ContractMethodToken() = default;

    /// Constructor from JSON
    explicit ContractMethodToken(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<ContractMethodToken> fromJson(const nlohmann::json& json) {
        return std::make_shared<ContractMethodToken>(json);
    }
};

} // namespace neocpp
