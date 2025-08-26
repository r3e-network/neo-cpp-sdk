#ifndef NEOCPP_PROTOCOL_CORE_RESPONSE_EXPRESS_SHUTDOWN_HPP
#define NEOCPP_PROTOCOL_CORE_RESPONSE_EXPRESS_SHUTDOWN_HPP

#include <string>
#include <nlohmann/json.hpp>
#include <neocpp/protocol/core/response.hpp>

namespace neocpp {

/**
 * Response type for express_shutdown RPC method
 * Used to shut down a Neo Express private blockchain instance
 */
class ExpressShutdown : public Response<ExpressShutdown> {
public:
    struct ShutdownInfo {
        int processId;

        ShutdownInfo() : processId(0) {}
        
        explicit ShutdownInfo(int pid) : processId(pid) {}

        bool operator==(const ShutdownInfo& other) const {
            return processId == other.processId;
        }
    };

    ExpressShutdown() = default;
    
    explicit ExpressShutdown(const nlohmann::json& j) {
        from_json(j);
    }

    int getProcessId() const {
        return result.has_value() ? result->processId : 0;
    }

    void setProcessId(int pid) {
        result = ShutdownInfo(pid);
    }

    void from_json(const nlohmann::json& j) override {
        Response<ExpressShutdown>::from_json(j);
        
        if (j.contains("result") && !j["result"].is_null()) {
            auto resultJson = j["result"];
            ShutdownInfo info;
            
            // Handle both "processId" and "process-id" keys
            if (resultJson.contains("process-id")) {
                // Handle string-encoded integer
                if (resultJson["process-id"].is_string()) {
                    info.processId = std::stoi(resultJson["process-id"].get<std::string>());
                } else {
                    info.processId = resultJson["process-id"].get<int>();
                }
            } else if (resultJson.contains("processId")) {
                if (resultJson["processId"].is_string()) {
                    info.processId = std::stoi(resultJson["processId"].get<std::string>());
                } else {
                    info.processId = resultJson["processId"].get<int>();
                }
            }
            
            result = info;
        }
    }

    nlohmann::json to_json() const override {
        auto j = Response<ExpressShutdown>::to_json();
        
        if (result.has_value()) {
            nlohmann::json resultJson;
            resultJson["process-id"] = std::to_string(result->processId);
            j["result"] = resultJson;
        }
        
        return j;
    }

private:
    std::optional<ShutdownInfo> result;
};

} // namespace neocpp

#endif // NEOCPP_PROTOCOL_CORE_RESPONSE_EXPRESS_SHUTDOWN_HPP