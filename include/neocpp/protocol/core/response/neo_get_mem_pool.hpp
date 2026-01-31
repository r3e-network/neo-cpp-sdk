#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoGetMemPool response type
class NeoGetMemPool {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoGetMemPool() = default;

    /// Constructor from JSON
    explicit NeoGetMemPool(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoGetMemPool> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoGetMemPool>(json);
    }
};

} // namespace neocpp
