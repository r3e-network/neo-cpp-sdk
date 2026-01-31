#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// OracleRequest response type
class OracleRequest {
private:
    nlohmann::json data_;

public:
    /// Constructor
    OracleRequest() = default;

    /// Constructor from JSON
    explicit OracleRequest(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<OracleRequest> fromJson(const nlohmann::json& json) {
        return std::make_shared<OracleRequest>(json);
    }
};

} // namespace neocpp
