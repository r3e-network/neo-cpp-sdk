#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// JSON-RPC 2.0 Request
class Request {
private:
    std::string jsonrpc_;
    std::string method_;
    nlohmann::json params_;
    int id_;
    static int nextId_;

public:
    /// Constructor
    Request();

    /// Constructor with method
    /// @param method The RPC method name
    explicit Request(const std::string& method);

    /// Constructor with method and params
    /// @param method The RPC method name
    /// @param params The parameters
    Request(const std::string& method, const nlohmann::json& params);

    /// Get JSON-RPC version
    const std::string& getJsonRpc() const { return jsonrpc_; }

    /// Get method
    const std::string& getMethod() const { return method_; }
    void setMethod(const std::string& method) { method_ = method; }

    /// Get params
    const nlohmann::json& getParams() const { return params_; }
    void setParams(const nlohmann::json& params) { params_ = params; }

    /// Get ID
    [[nodiscard]] int getId() const { return id_; }
    void setId(int id) { id_ = id; }

    /// Convert to JSON
    nlohmann::json toJson() const;

    /// Convert to string
    std::string toString() const;

    /// Create from JSON
    static SharedPtr<Request> fromJson(const nlohmann::json& json);

private:
    /// Generate next ID
    static int generateId();
};

} // namespace neocpp
