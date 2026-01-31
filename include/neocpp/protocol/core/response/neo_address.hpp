#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoAddress response type
class NeoAddress {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoAddress() = default;

    /// Constructor from JSON
    explicit NeoAddress(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoAddress> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoAddress>(json);
    }
};

} // namespace neocpp
