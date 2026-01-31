#pragma once


#include <nlohmann/json.hpp>
#include <neocpp/types/hash160.hpp>
#include <neocpp/protocol/core/response/contract_manifest.hpp>

namespace neocpp {

/**
 * Express-specific contract state representation
 * Used by Neo Express private blockchain for contract management
 */
class ExpressContractState {
public:
    Hash160 hash;
    ContractManifest manifest;

    ExpressContractState() = default;

    ExpressContractState(const Hash160& hash, const ContractManifest& manifest)
        : hash(hash), manifest(manifest) {}

    bool operator==(const ExpressContractState& other) const {
        return hash == other.hash && manifest == other.manifest;
    }

    bool operator!=(const ExpressContractState& other) const {
        return !(*this == other);
    }

    void from_json(const nlohmann::json& j) {
        if (j.contains("hash")) {
            hash = Hash160(j["hash"].get<std::string>());
        }
        if (j.contains("manifest")) {
            manifest.from_json(j["manifest"]);
        }
    }

    nlohmann::json to_json() const {
        nlohmann::json j;
        j["hash"] = hash.toString();
        j["manifest"] = manifest.to_json();
        return j;
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ExpressContractState, hash, manifest)
};

} // namespace neocpp

