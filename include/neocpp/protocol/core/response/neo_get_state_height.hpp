#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetStateHeight response type
class NeoGetStateHeight {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetStateHeight() = default;

    /// Constructor from JSON
    explicit NeoGetStateHeight(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetStateHeight> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetStateHeight>(json);
    }
};

} // namespace neocpp
