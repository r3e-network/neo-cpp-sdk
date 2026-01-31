
#include "neocpp/protocol/core/response/invocation_result.hpp"
#include "neocpp/protocol/stack_item.hpp"

namespace neocpp {

InvocationResult::InvocationResult(const nlohmann::json& json) : gasConsumed_(0) {
    if (json.contains("script")) {
        script_ = json["script"].get<std::string>();
    }

    if (json.contains("state")) {
        state_ = json["state"].get<std::string>();
    }

    if (json.contains("gasconsumed")) {
        std::string gasStr = json["gasconsumed"].get<std::string>();
        gasConsumed_ = std::stoll(gasStr);
    }

    if (json.contains("exception") && !json["exception"].is_null()) {
        exception_ = json["exception"].get<std::string>();
    }

    if (json.contains("stack")) {
        for (const auto& item : json["stack"]) {
            stack_.push_back(StackItem::fromJson(item));
        }
    }

    if (json.contains("notifications")) {
        for (const auto& notification : json["notifications"]) {
            notifications_.push_back(notification);
        }
    }

    if (json.contains("session")) {
        sessionId_ = json["session"].get<std::string>();
    }
} // namespace neocpp
nlohmann::json InvocationResult::toJson() const {
    nlohmann::json json;

    json["script"] = script_;
    json["state"] = state_;
    json["gasconsumed"] = std::to_string(gasConsumed_);

    if (!exception_.empty()) {
        json["exception"] = exception_;
    }

    nlohmann::json stackArray = nlohmann::json::array();
    for (const auto& item : stack_) {
        stackArray.push_back(item->toJson());
    }
    json["stack"] = stackArray;

    json["notifications"] = notifications_;

    if (!sessionId_.empty()) {
        json["session"] = sessionId_;
    }

    return json;
} // namespace neocpp
SharedPtr<InvocationResult> InvocationResult::fromJson(const nlohmann::json& json) {
    return std::make_shared<InvocationResult>(json);
} // namespace neocpp
} // namespace neocpp
