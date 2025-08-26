#ifndef NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_WALLET_BALANCE_HPP
#define NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_WALLET_BALANCE_HPP

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include <neocpp/protocol/core/response.hpp>

namespace neocpp {

/**
 * Response type for getwalletbalance RPC method
 * Returns the balance of a specific asset in the wallet
 */
class NeoGetWalletBalance : public Response<NeoGetWalletBalance> {
public:
    struct Balance {
        std::string balance;

        Balance() = default;
        
        explicit Balance(const std::string& balance) : balance(balance) {}

        bool operator==(const Balance& other) const {
            return balance == other.balance;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Balance, balance)
    };

    NeoGetWalletBalance() = default;
    
    explicit NeoGetWalletBalance(const nlohmann::json& j) {
        from_json(j);
    }

    std::optional<Balance> getWalletBalance() const {
        return result;
    }

    void setWalletBalance(const Balance& balance) {
        result = balance;
    }

    void from_json(const nlohmann::json& j) override {
        Response<NeoGetWalletBalance>::from_json(j);
        
        if (j.contains("result") && !j["result"].is_null()) {
            auto resultJson = j["result"];
            Balance balance;
            
            // Handle both lowercase and uppercase "balance" key
            if (resultJson.contains("balance")) {
                balance.balance = resultJson["balance"].get<std::string>();
            } else if (resultJson.contains("Balance")) {
                balance.balance = resultJson["Balance"].get<std::string>();
            } else if (resultJson.is_object() && resultJson.size() == 1) {
                // Handle case where result is directly the balance object
                auto it = resultJson.begin();
                if (it.key() == "balance" || it.key() == "Balance") {
                    balance.balance = it.value().get<std::string>();
                }
            }
            
            result = balance;
        }
    }

    nlohmann::json to_json() const override {
        auto j = Response<NeoGetWalletBalance>::to_json();
        
        if (result.has_value()) {
            nlohmann::json resultJson;
            resultJson["balance"] = result->balance;
            j["result"] = resultJson;
        }
        
        return j;
    }

private:
    std::optional<Balance> result;
};

} // namespace neocpp

#endif // NEOCPP_PROTOCOL_CORE_RESPONSE_NEO_GET_WALLET_BALANCE_HPP