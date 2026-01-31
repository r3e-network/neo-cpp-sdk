#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoBlock response type
class NeoBlock {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoBlock() = default;

    /// Constructor from JSON
    explicit NeoBlock(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoBlock> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoBlock>(json);
    }
};

} // namespace neocpp
