#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"

namespace neocpp {

/// JSON-RPC 2.0 Response
class Response {
public:
    /// Error structure
    struct Error {
        int code;
        std::string message;
        nlohmann::json data;

        Error() : code(0) {}
        Error(int c, const std::string& m) : code(c), message(m) {}
        Error(int c, const std::string& m, const nlohmann::json& d)
            : code(c), message(m), data(d) {}
    };

private:
    std::string jsonrpc_;
    nlohmann::json result_;
    std::unique_ptr<Error> error_;
    int id_;

public:
    /// Constructor
    Response();

    /// Constructor with result
    /// @param result The result
    explicit Response(const nlohmann::json& result);

    /// Constructor with error
    /// @param error The error
    explicit Response(const Error& error);

    /// Get JSON-RPC version
    const std::string& getJsonRpc() const { return jsonrpc_; }

    /// Get result
    const nlohmann::json& getResult() const { return result_; }
    void setResult(const nlohmann::json& result) { result_ = result; }

    /// Check if has error
    [[nodiscard]] bool hasError() const { return error_ != nullptr; }

    /// Get error
    const Error* getError() const { return error_.get(); }
    void setError(const Error& error) { error_ = std::make_unique<Error>(error); }

    /// Get ID
    [[nodiscard]] int getId() const { return id_; }
    void setId(int id) { id_ = id; }

    /// Convert to JSON
    nlohmann::json toJson() const;

    /// Convert to string
    std::string toString() const;

    /// Create from JSON
    static SharedPtr<Response> fromJson(const nlohmann::json& json);

    /// Check if response is successful
    [[nodiscard]] bool isSuccess() const { return !hasError(); }

    /// Get result as specific type
    template<typename T>
    [[nodiscard]] T getResultAs() const {
        return result_.get<T>();
    }
};

} // namespace neocpp
