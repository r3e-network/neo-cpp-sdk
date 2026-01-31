#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// NeoSendRawTransaction response type
class NeoSendRawTransaction {
private:
    nlohmann::json data_;

public:
    /// Constructor
    NeoSendRawTransaction() = default;

    /// Constructor from JSON
    explicit NeoSendRawTransaction(const nlohmann::json& json) : data_(json) {}

    /// Get raw JSON data
    const nlohmann::json& getRawJson() const { return data_; }

    /// Convert to JSON
    nlohmann::json toJson() const { return data_; }

    /// Create from JSON
    static SharedPtr<NeoSendRawTransaction> fromJson(const nlohmann::json& json) {
        return std::make_shared<NeoSendRawTransaction>(json);
    }
};

} // namespace neocpp
