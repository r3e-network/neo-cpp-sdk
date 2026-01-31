#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoFindStates response type
class NeoFindStates {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoFindStates() = default;

    /// Constructor from JSON
    explicit NeoFindStates(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoFindStates> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoFindStates>(json);
    }
};

} // namespace neocpp
