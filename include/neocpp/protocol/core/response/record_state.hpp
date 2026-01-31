#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// RecordState response type
class RecordState {
private:
    nlohmann::json data_;

public:
    /// Constructor
    RecordState() = default;

    /// Constructor from JSON
    explicit RecordState(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<RecordState> fromJson(const nlohmann::json& json) {
        return std::make_shared<RecordState>(json);
    }
};

} // namespace neocpp
