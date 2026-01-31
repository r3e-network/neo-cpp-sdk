#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetNep17Balances response type
class NeoGetNep17Balances {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetNep17Balances() = default;

    /// Constructor from JSON
    explicit NeoGetNep17Balances(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetNep17Balances> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetNep17Balances>(json);
    }
};

} // namespace neocpp
