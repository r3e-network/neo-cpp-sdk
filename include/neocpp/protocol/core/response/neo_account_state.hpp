#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoAccountState response type
class NeoAccountState {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoAccountState() = default;

    /// Constructor from JSON
    explicit NeoAccountState(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoAccountState> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoAccountState>(json);
    }
};

} // namespace neocpp
