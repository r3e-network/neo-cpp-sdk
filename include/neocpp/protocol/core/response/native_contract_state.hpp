#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NativeContractState response type
class NativeContractState {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NativeContractState() = default;

    /// Constructor from JSON
    explicit NativeContractState(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NativeContractState> fromJson(const nlohmann::json& json) {
        return std::make_shared<NativeContractState>(json);
    }
};

} // namespace neocpp
