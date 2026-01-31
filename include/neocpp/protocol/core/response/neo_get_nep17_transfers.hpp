#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetNep17Transfers response type
class NeoGetNep17Transfers {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetNep17Transfers() = default;

    /// Constructor from JSON
    explicit NeoGetNep17Transfers(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetNep17Transfers> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetNep17Transfers>(json);
    }
};

} // namespace neocpp
