#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetUnspents response type
class NeoGetUnspents {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetUnspents() = default;

    /// Constructor from JSON
    explicit NeoGetUnspents(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetUnspents> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetUnspents>(json);
    }
};

} // namespace neocpp
