#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace neocpp {

/// HTTP response structure
struct HttpResponse {
    int statusCode;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    std::string error;

    bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
};

/// HTTP service for making requests
class HttpService {
private:
    std::string baseUrl_;

public:
    using Headers = std::unordered_map<std::string, std::string>;
    using ResponseCallback = std::function<void(const HttpResponse&)>;

    /// Constructor
    explicit HttpService(const std::string& baseUrl);

    /// Destructor
    ~HttpService();

    /// Get the base URL
    const std::string& getUrl() const { return baseUrl_; }

    /// Set timeout for requests
    /// @param seconds Timeout in seconds
    void setTimeout(int seconds);

    /// Set default headers
    /// @param headers The headers to set
    void setDefaultHeaders(const Headers& headers);

    /// Perform GET request
    /// @param url The URL
    /// @param headers Optional additional headers
    /// @return The response
    HttpResponse get(const std::string& url, const Headers& headers = {});

    /// Perform POST request
    /// @param url The URL
    /// @param body The request body
    /// @param headers Optional additional headers
    /// @return The response
    HttpResponse post(const std::string& url, const std::string& body, const Headers& headers = {});

    /// Perform JSON-RPC POST request
    /// @param data The JSON data
    /// @param endpoint Optional endpoint (default empty)
    /// @return The JSON response
    nlohmann::json post(const nlohmann::json& data, const std::string& endpoint = "");

    /// Perform JSON GET request
    /// @param endpoint The endpoint
    /// @return The JSON response
    nlohmann::json get(const std::string& endpoint);

    /// Perform PUT request
    /// @param url The URL
    /// @param body The request body
    /// @param headers Optional additional headers
    /// @return The response
    HttpResponse put(const std::string& url, const std::string& body, const Headers& headers = {});

    /// Perform DELETE request
    /// @param url The URL
    /// @param headers Optional additional headers
    /// @return The response
    HttpResponse del(const std::string& url, const Headers& headers = {});

    /// Perform async GET request
    /// @param url The URL
    /// @param callback The response callback
    /// @param headers Optional additional headers
    [[nodiscard]] void getAsync(const std::string& url, ResponseCallback callback, const Headers& headers = {});

    /// Perform async POST request
    /// @param url The URL
    /// @param body The request body
    /// @param callback The response callback
    /// @param headers Optional additional headers
    void postAsync(const std::string& url, const std::string& body, ResponseCallback callback, const Headers& headers = {});
};

} // namespace neocpp
