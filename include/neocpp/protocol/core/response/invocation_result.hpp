#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

// Forward declaration
class StackItem;

/// Result of a contract invocation
class InvocationResult {
private:
    std::string script_;
    std::string state_;
    int64_t gasConsumed_;
    std::string exception_;
    std::vector<SharedPtr<StackItem>> stack_;
    std::vector<nlohmann::json> notifications_;
    std::string sessionId_;

public:
    /// Constructor
    InvocationResult() : gasConsumed_(0) {}

    /// Constructor from JSON
    explicit InvocationResult(const nlohmann::json& json);

    /// Get script
    const std::string& getScript() const { return script_; }
    void setScript(const std::string& script) { script_ = script; }

    /// Get state
    const std::string& getState() const { return state_; }
    void setState(const std::string& state) { state_ = state; }

    /// Check if execution halted
    bool hasStateFault() const { return state_ == "FAULT"; }

    /// Get gas consumed
    [[nodiscard]] int64_t getGasConsumed() const { return gasConsumed_; }
    void setGasConsumed(int64_t gas) { gasConsumed_ = gas; }

    /// Get exception
    const std::string& getException() const { return exception_; }
    void setException(const std::string& exception) { exception_ = exception; }

    /// Get stack
    const std::vector<SharedPtr<StackItem>>& getStack() const { return stack_; }
    void setStack(const std::vector<SharedPtr<StackItem>>& stack) { stack_ = stack; }

    /// Get notifications
    const std::vector<nlohmann::json>& getNotifications() const { return notifications_; }
    void setNotifications(const std::vector<nlohmann::json>& notifications) { notifications_ = notifications; }

    /// Get session ID
    const std::string& getSessionId() const { return sessionId_; }
    void setSessionId(const std::string& sessionId) { sessionId_ = sessionId; }

    /// Convert to JSON
    nlohmann::json toJson() const;

    /// Create from JSON
    static SharedPtr<InvocationResult> fromJson(const nlohmann::json& json);
};

} // namespace neocpp
