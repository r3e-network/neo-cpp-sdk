#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetUnclaimedGas response type
class NeoGetUnclaimedGas {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetUnclaimedGas() = default;

    /// Constructor from JSON
    explicit NeoGetUnclaimedGas(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetUnclaimedGas> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetUnclaimedGas>(json);
    }
};

} // namespace neocpp
