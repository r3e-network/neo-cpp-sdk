#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// ContractNef response type
class ContractNef {
private:
    nlohmann::json data_;

public:
    /// Constructor
    ContractNef() = default;

    /// Constructor from JSON
    explicit ContractNef(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<ContractNef> fromJson(const nlohmann::json& json) {
        return std::make_shared<ContractNef>(json);
    }
};

} // namespace neocpp
