#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoNetworkFee response type
class NeoNetworkFee {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoNetworkFee() = default;

    /// Constructor from JSON
    explicit NeoNetworkFee(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoNetworkFee> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoNetworkFee>(json);
    }
};

} // namespace neocpp
