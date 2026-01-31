#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// ContractState response type
class ContractState {
private:
    nlohmann::json data_;

public:
    /// Constructor
    ContractState() = default;

    /// Constructor from JSON
    explicit ContractState(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<ContractState> fromJson(const nlohmann::json& json) {
        return std::make_shared<ContractState>(json);
    }
};

} // namespace neocpp
