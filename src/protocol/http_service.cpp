
#include "neocpp/protocol/http_service.hpp"
#include "neocpp/exceptions.hpp"
#include <sstream>
#include <mutex>
#include <cstdlib>

#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

namespace neocpp {

#ifdef HAVE_CURL
// Callback for CURL to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
} // namespace neocpp
#endif

HttpService::HttpService(const std::string& baseUrl)
    : baseUrl_(baseUrl) {
#ifdef HAVE_CURL
    // Thread-safe CURL initialization using std::once_flag
    static std::once_flag curlInitFlag;
    std::call_once(curlInitFlag, []() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        // Register cleanup handler
        std::atexit([]() { curl_global_cleanup(); });
    });
#endif
} // namespace neocpp
HttpService::~HttpService() {
    // Cleanup is handled globally
} // namespace neocpp
nlohmann::json HttpService::post(const nlohmann::json& data, const std::string& endpoint) {
#ifdef HAVE_CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw RpcException("Failed to initialize CURL");
    }

    std::string url = baseUrl_ + endpoint;
    std::string jsonStr = data.dump();
    std::string response;

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonStr.length());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Set headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform request
    CURLcode res = curl_easy_perform(curl);

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw RpcException("HTTP request failed: " + std::string(curl_easy_strerror(res)));
    }

    // Parse response
    try {
        return nlohmann::json::parse(response);
    } catch (const nlohmann::json::exception& e) {
        throw RpcException("Failed to parse JSON response: " + std::string(e.what()));
    }
#else
    (void)data;
    (void)endpoint;
    throw RpcException("HTTP support not available (CURL not found)");
#endif
} // namespace neocpp
nlohmann::json HttpService::get(const std::string& endpoint) {
#ifdef HAVE_CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw RpcException("Failed to initialize CURL");
    }

    std::string url = baseUrl_ + endpoint;
    std::string response;

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Set headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform request
    CURLcode res = curl_easy_perform(curl);

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw RpcException("HTTP request failed: " + std::string(curl_easy_strerror(res)));
    }

    // Parse response
    try {
        return nlohmann::json::parse(response);
    } catch (const nlohmann::json::exception& e) {
        throw RpcException("Failed to parse JSON response: " + std::string(e.what()));
    }
#else
    (void)endpoint;
    throw RpcException("HTTP support not available (CURL not found)");
#endif
} // namespace neocpp
} // namespace neocpp
