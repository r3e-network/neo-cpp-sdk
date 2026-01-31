#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// Contract manifest containing ABI and metadata
class ContractManifest {
public:
    struct Parameter {
        std::string name;
        std::string type;
    };

    struct Method {
        std::string name;
        std::vector<Parameter> parameters;
        Parameter returnType;
        int offset;
        bool safe;
    };

    struct Event {
        std::string name;
        std::vector<Parameter> parameters;
    };

    struct Permission {
        std::string contract;
        std::vector<std::string> methods;
    };

    struct Group {
        std::string pubkey;
        std::string signature;
    };

    struct Feature {
        std::string name;
        nlohmann::json value;
    };

private:
    std::string name_;
    std::vector<Group> groups_;
    std::vector<Feature> features_;
    std::vector<std::string> supportedStandards_;
    std::vector<Permission> permissions_;
    std::vector<std::string> trusts_;
    std::vector<Method> methods_;
    std::vector<Event> events_;
    nlohmann::json extra_;

public:
    /// Constructor
    ContractManifest() = default;

    /// Constructor from JSON
    explicit ContractManifest(const nlohmann::json& json);

    /// Get name
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    /// Get groups
    const std::vector<Group>& getGroups() const { return groups_; }
    void setGroups(const std::vector<Group>& groups) { groups_ = groups; }

    /// Get features
    const std::vector<Feature>& getFeatures() const { return features_; }
    void setFeatures(const std::vector<Feature>& features) { features_ = features; }

    /// Get supported standards
    const std::vector<std::string>& getSupportedStandards() const { return supportedStandards_; }
    void setSupportedStandards(const std::vector<std::string>& standards) { supportedStandards_ = standards; }

    /// Get permissions
    const std::vector<Permission>& getPermissions() const { return permissions_; }
    void setPermissions(const std::vector<Permission>& permissions) { permissions_ = permissions; }

    /// Get trusts
    const std::vector<std::string>& getTrusts() const { return trusts_; }
    void setTrusts(const std::vector<std::string>& trusts) { trusts_ = trusts; }

    /// Get methods
    const std::vector<Method>& getMethods() const { return methods_; }
    void setMethods(const std::vector<Method>& methods) { methods_ = methods; }

    /// Get events
    const std::vector<Event>& getEvents() const { return events_; }
    void setEvents(const std::vector<Event>& events) { events_ = events; }

    /// Get extra data
    const nlohmann::json& getExtra() const { return extra_; }
    void setExtra(const nlohmann::json& extra) { extra_ = extra; }

    /// Convert to JSON
    nlohmann::json toJson() const;

    /// Create from JSON
    static SharedPtr<ContractManifest> fromJson(const nlohmann::json& json);

    /// Check if standard is supported
    bool supportsStandard(const std::string& standard) const;

    /// Get method by name
    const Method* getMethod(const std::string& name) const;

    /// Get event by name
    const Event* getEvent(const std::string& name) const;
};

} // namespace neocpp
