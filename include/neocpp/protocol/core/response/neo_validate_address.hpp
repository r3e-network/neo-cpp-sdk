#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoValidateAddress response type
class NeoValidateAddress {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoValidateAddress() = default;

    /// Constructor from JSON
    explicit NeoValidateAddress(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoValidateAddress> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoValidateAddress>(json);
    }
};

} // namespace neocpp
