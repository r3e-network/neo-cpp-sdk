#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetVersion response type
class NeoGetVersion {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetVersion() = default;

    /// Constructor from JSON
    explicit NeoGetVersion(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetVersion> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetVersion>(json);
    }
};

} // namespace neocpp
