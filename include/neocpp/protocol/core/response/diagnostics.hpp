#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Diagnostics response type
class Diagnostics {
private:
    nlohmann::json data_;

public:
    /// Constructor
    Diagnostics() = default;

    /// Constructor from JSON
    explicit Diagnostics(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<Diagnostics> fromJson(const nlohmann::json& json) {
        return std::make_shared<Diagnostics>(json);
    }
};

} // namespace neocpp
