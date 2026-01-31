#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoListPlugins response type
class NeoListPlugins {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoListPlugins() = default;

    /// Constructor from JSON
    explicit NeoListPlugins(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoListPlugins> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoListPlugins>(json);
    }
};

} // namespace neocpp
