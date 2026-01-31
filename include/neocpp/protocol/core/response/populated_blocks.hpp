#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// PopulatedBlocks response type
class PopulatedBlocks {
private:
    nlohmann::json data_;

public:
    /// Constructor
    PopulatedBlocks() = default;

    /// Constructor from JSON
    explicit PopulatedBlocks(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<PopulatedBlocks> fromJson(const nlohmann::json& json) {
        return std::make_shared<PopulatedBlocks>(json);
    }
};

} // namespace neocpp
