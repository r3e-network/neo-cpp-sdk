#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoApplicationLog response type
class NeoApplicationLog {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoApplicationLog() = default;

    /// Constructor from JSON
    explicit NeoApplicationLog(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoApplicationLog> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoApplicationLog>(json);
    }
};

} // namespace neocpp
