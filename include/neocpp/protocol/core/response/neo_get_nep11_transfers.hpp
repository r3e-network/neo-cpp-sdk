#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetNep11Transfers response type
class NeoGetNep11Transfers {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetNep11Transfers() = default;

    /// Constructor from JSON
    explicit NeoGetNep11Transfers(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetNep11Transfers> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetNep11Transfers>(json);
    }
};

} // namespace neocpp
