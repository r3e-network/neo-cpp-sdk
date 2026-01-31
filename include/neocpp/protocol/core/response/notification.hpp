#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Notification response type
class Notification {
private:
    nlohmann::json data_;

public:
    /// Constructor
    Notification() = default;

    /// Constructor from JSON
    explicit Notification(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<Notification> fromJson(const nlohmann::json& json) {
        return std::make_shared<Notification>(json);
    }
};

} // namespace neocpp
