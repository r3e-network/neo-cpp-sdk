#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// ContractStorageEntry response type
class ContractStorageEntry {
private:
    nlohmann::json data_;

public:
    /// Constructor
    ContractStorageEntry() = default;

    /// Constructor from JSON
    explicit ContractStorageEntry(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<ContractStorageEntry> fromJson(const nlohmann::json& json) {
        return std::make_shared<ContractStorageEntry>(json);
    }
};

} // namespace neocpp
