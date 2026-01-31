
#include "neocpp/protocol/core/response/contract_manifest.hpp"
#include <algorithm>

namespace neocpp {

ContractManifest::ContractManifest(const nlohmann::json& json) {
    if (json.contains("name")) {
        name_ = json["name"].get<std::string>();
    }

    if (json.contains("groups")) {
        for (const auto& group : json["groups"]) {
            Group g;
            g.pubkey = group["pubkey"].get<std::string>();
            g.signature = group["signature"].get<std::string>();
            groups_.push_back(g);
        }
    }

    if (json.contains("features")) {
        for (const auto& [key, value] : json["features"].items()) {
            Feature f;
            f.name = key;
            f.value = value;
            features_.push_back(f);
        }
    }

    if (json.contains("supportedstandards")) {
        for (const auto& standard : json["supportedstandards"]) {
            supportedStandards_.push_back(standard.get<std::string>());
        }
    }

    if (json.contains("permissions")) {
        for (const auto& perm : json["permissions"]) {
            Permission p;
            p.contract = perm["contract"].get<std::string>();
            for (const auto& method : perm["methods"]) {
                p.methods.push_back(method.get<std::string>());
            }
            permissions_.push_back(p);
        }
    }

    if (json.contains("trusts")) {
        for (const auto& trust : json["trusts"]) {
            trusts_.push_back(trust.get<std::string>());
        }
    }

    if (json.contains("abi")) {
        const auto& abi = json["abi"];

        if (abi.contains("methods")) {
            for (const auto& method : abi["methods"]) {
                Method m;
                m.name = method["name"].get<std::string>();
                m.offset = method["offset"].get<int>();
                m.safe = method.value("safe", false);

                if (method.contains("parameters")) {
                    for (const auto& param : method["parameters"]) {
                        Parameter p;
                        p.name = param["name"].get<std::string>();
                        p.type = param["type"].get<std::string>();
                        m.parameters.push_back(p);
                    }
                }

                if (method.contains("returntype")) {
                    m.returnType.type = method["returntype"].get<std::string>();
                }

                methods_.push_back(m);
            }
        }

        if (abi.contains("events")) {
            for (const auto& event : abi["events"]) {
                Event e;
                e.name = event["name"].get<std::string>();

                if (event.contains("parameters")) {
                    for (const auto& param : event["parameters"]) {
                        Parameter p;
                        p.name = param["name"].get<std::string>();
                        p.type = param["type"].get<std::string>();
                        e.parameters.push_back(p);
                    }
                }

                events_.push_back(e);
            }
        }
    }

    if (json.contains("extra")) {
        extra_ = json["extra"];
    }
} // namespace neocpp
nlohmann::json ContractManifest::toJson() const {
    nlohmann::json json;

    json["name"] = name_;

    nlohmann::json groups = nlohmann::json::array();
    for (const auto& group : groups_) {
        nlohmann::json g;
        g["pubkey"] = group.pubkey;
        g["signature"] = group.signature;
        groups.push_back(g);
    }
    json["groups"] = groups;

    nlohmann::json features;
    for (const auto& feature : features_) {
        features[feature.name] = feature.value;
    }
    json["features"] = features;

    json["supportedstandards"] = supportedStandards_;

    nlohmann::json permissions = nlohmann::json::array();
    for (const auto& perm : permissions_) {
        nlohmann::json p;
        p["contract"] = perm.contract;
        p["methods"] = perm.methods;
        permissions.push_back(p);
    }
    json["permissions"] = permissions;

    json["trusts"] = trusts_;

    nlohmann::json abi;

    nlohmann::json methods = nlohmann::json::array();
    for (const auto& method : methods_) {
        nlohmann::json m;
        m["name"] = method.name;
        m["offset"] = method.offset;
        m["safe"] = method.safe;

        nlohmann::json params = nlohmann::json::array();
        for (const auto& param : method.parameters) {
            nlohmann::json p;
            p["name"] = param.name;
            p["type"] = param.type;
            params.push_back(p);
        }
        m["parameters"] = params;

        m["returntype"] = method.returnType.type;
        methods.push_back(m);
    }
    abi["methods"] = methods;

    nlohmann::json events = nlohmann::json::array();
    for (const auto& event : events_) {
        nlohmann::json e;
        e["name"] = event.name;

        nlohmann::json params = nlohmann::json::array();
        for (const auto& param : event.parameters) {
            nlohmann::json p;
            p["name"] = param.name;
            p["type"] = param.type;
            params.push_back(p);
        }
        e["parameters"] = params;
        events.push_back(e);
    }
    abi["events"] = events;

    json["abi"] = abi;

    if (!extra_.is_null()) {
        json["extra"] = extra_;
    }

    return json;
} // namespace neocpp
SharedPtr<ContractManifest> ContractManifest::fromJson(const nlohmann::json& json) {
    return std::make_shared<ContractManifest>(json);
} // namespace neocpp
bool ContractManifest::supportsStandard(const std::string& standard) const {
    return std::find(supportedStandards_.begin(), supportedStandards_.end(), standard) != supportedStandards_.end();
} // namespace neocpp
const ContractManifest::Method* ContractManifest::getMethod(const std::string& name) const {
    for (const auto& method : methods_) {
        if (method.name == name) {
            return &method;
        }
    }
    return nullptr;
} // namespace neocpp
const ContractManifest::Event* ContractManifest::getEvent(const std::string& name) const {
    for (const auto& event : events_) {
        if (event.name == name) {
            return &event;
        }
    }
    return nullptr;
} // namespace neocpp
} // namespace neocpp
