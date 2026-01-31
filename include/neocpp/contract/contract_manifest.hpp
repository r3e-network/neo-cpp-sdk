#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Contract manifest containing metadata and ABI
class ContractManifest {
private:
    std::string name_;
    std::vector<std::string> groups_;
    std::vector<std::string> supportedStandards_;
    nlohmann::json abi_;
    nlohmann::json permissions_;
    nlohmann::json trusts_;
    nlohmann::json extra_;

public:
    /// Constructor
    ContractManifest();

    /// Constructor from JSON
    /// @param json The manifest JSON
    explicit ContractManifest(const nlohmann::json& json);

    /// Destructor
    ~ContractManifest() = default;

    // Getters
    const std::string& getName() const { return name_; }
    const std::vector<std::string>& getGroups() const { return groups_; }
    const std::vector<std::string>& getSupportedStandards() const { return supportedStandards_; }
    const nlohmann::json& getAbi() const { return abi_; }
    const nlohmann::json& getPermissions() const { return permissions_; }
    const nlohmann::json& getTrusts() const { return trusts_; }
    const nlohmann::json& getExtra() const { return extra_; }

    // Setters
    void setName(const std::string& name) { name_ = name; }
    void setAbi(const nlohmann::json& abi) { abi_ = abi; }
    void setPermissions(const nlohmann::json& permissions) { permissions_ = permissions; }
    void setTrusts(const nlohmann::json& trusts) { trusts_ = trusts; }
    void setExtra(const nlohmann::json& extra) { extra_ = extra; }

    /// Add a group
    void addGroup(const std::string& group) { groups_.push_back(group); }

    /// Add a supported standard
    void addSupportedStandard(const std::string& standard) { supportedStandards_.push_back(standard); }

    /// Get contract methods
    /// @return List of methods from ABI
    std::vector<nlohmann::json> getMethods() const;

    /// Get contract events
    /// @return List of events from ABI
    std::vector<nlohmann::json> getEvents() const;

    /// Check if supports standard
    /// @param standard The standard name (e.g., "NEP-17")
    /// @return True if supported
    bool supportsStandard(const std::string& standard) const;

    /// Convert to JSON
    /// @return The manifest as JSON
    nlohmann::json toJson() const;

    /// Parse from JSON
    /// @param json The manifest JSON
    /// @return The contract manifest
    static ContractManifest fromJson(const nlohmann::json& json);
};

} // namespace neocpp
