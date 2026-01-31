#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetStateRoot response type
class NeoGetStateRoot {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetStateRoot() = default;

    /// Constructor from JSON
    explicit NeoGetStateRoot(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetStateRoot> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetStateRoot>(json);
    }
};

} // namespace neocpp
