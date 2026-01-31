#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Nep17Contract response type
class Nep17Contract {
private:
    nlohmann::json data_;

public:
    /// Constructor
    Nep17Contract() = default;

    /// Constructor from JSON
    explicit Nep17Contract(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<Nep17Contract> fromJson(const nlohmann::json& json) {
        return std::make_shared<Nep17Contract>(json);
    }
};

} // namespace neocpp
