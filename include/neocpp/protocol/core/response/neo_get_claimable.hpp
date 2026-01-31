#pragma once


#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include <neocpp/protocol/core/response.hpp>

namespace neocpp {

/**
 * Response type for getclaimable RPC method
 * Contains information about claimable GAS from NEO holdings
 */
class NeoGetClaimable : public Response<NeoGetClaimable> {
public:
    struct Claim {
        std::string txId;
        int index;
        int neoValue;
        int startHeight;
        int endHeight;
        std::string generatedGas;
        std::string systemFee;
        std::string unclaimedGas;

        Claim() = default;

        Claim(const std::string& txId, int index, int neoValue,
              int startHeight, int endHeight,
              const std::string& generatedGas, const std::string& systemFee,
              const std::string& unclaimedGas)
            : txId(txId), index(index), neoValue(neoValue),
              startHeight(startHeight), endHeight(endHeight),
              generatedGas(generatedGas), systemFee(systemFee),
              unclaimedGas(unclaimedGas) {}

        bool operator==(const Claim& other) const {
            return txId == other.txId &&
                   index == other.index &&
                   neoValue == other.neoValue &&
                   startHeight == other.startHeight &&
                   endHeight == other.endHeight &&
                   generatedGas == other.generatedGas &&
                   systemFee == other.systemFee &&
                   unclaimedGas == other.unclaimedGas;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Claim,
            txId, index, neoValue, startHeight, endHeight,
            generatedGas, systemFee, unclaimedGas)
    };

    struct Claimables {
        std::vector<Claim> claims;
        std::string address;
        std::string totalUnclaimed;

        Claimables() = default;

        Claimables(const std::vector<Claim>& claims,
                   const std::string& address,
                   const std::string& totalUnclaimed)
            : claims(claims), address(address), totalUnclaimed(totalUnclaimed) {}

        bool operator==(const Claimables& other) const {
            return claims == other.claims &&
                   address == other.address &&
                   totalUnclaimed == other.totalUnclaimed;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Claimables, claims, address, totalUnclaimed)
    };

    NeoGetClaimable() = default;

    explicit NeoGetClaimable(const nlohmann::json& j) {
        from_json(j);
    }

    std::optional<Claimables> getClaimables() const {
        return result;
    }

    void setClaimables(const Claimables& claimables) {
        result = claimables;
    }

    void from_json(const nlohmann::json& j) override {
        Response<NeoGetClaimable>::from_json(j);

        if (j.contains("result") && !j["result"].is_null()) {
            auto resultJson = j["result"];

            Claimables claimables;
            claimables.address = resultJson.value("address", "");
            claimables.totalUnclaimed = resultJson.value("unclaimed", "");

            if (resultJson.contains("claimable")) {
                for (const auto& claimJson : resultJson["claimable"]) {
                    Claim claim;
                    claim.txId = claimJson.value("txid", "");
                    claim.index = claimJson.value("n", 0);
                    claim.neoValue = claimJson.value("value", 0);
                    claim.startHeight = claimJson.value("start_height", 0);
                    claim.endHeight = claimJson.value("end_height", 0);
                    claim.generatedGas = claimJson.value("generated", "");
                    claim.systemFee = claimJson.value("sysfee", "");
                    claim.unclaimedGas = claimJson.value("unclaimed", "");
                    claimables.claims.push_back(claim);
                }
            }

            result = claimables;
        }
    }

    nlohmann::json to_json() const override {
        auto j = Response<NeoGetClaimable>::to_json();

        if (result.has_value()) {
            nlohmann::json resultJson;
            resultJson["address"] = result->address;
            resultJson["unclaimed"] = result->totalUnclaimed;

            nlohmann::json claimsArray = nlohmann::json::array();
            for (const auto& claim : result->claims) {
                nlohmann::json claimJson;
                claimJson["txid"] = claim.txId;
                claimJson["n"] = claim.index;
                claimJson["value"] = claim.neoValue;
                claimJson["start_height"] = claim.startHeight;
                claimJson["end_height"] = claim.endHeight;
                claimJson["generated"] = claim.generatedGas;
                claimJson["sysfee"] = claim.systemFee;
                claimJson["unclaimed"] = claim.unclaimedGas;
                claimsArray.push_back(claimJson);
            }
            resultJson["claimable"] = claimsArray;

            j["result"] = resultJson;
        }

        return j;
    }

private:
    std::optional<Claimables> result;
};

} // namespace neocpp

