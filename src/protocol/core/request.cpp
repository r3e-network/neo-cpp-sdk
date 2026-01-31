
#include "neocpp/protocol/core/request.hpp"

namespace neocpp {

int Request::nextId_ = 1;

Request::Request() : jsonrpc_("2.0"), id_(generateId()) {
} // namespace neocpp
Request::Request(const std::string& method)
    : jsonrpc_("2.0"), method_(method), params_(nlohmann::json::array()), id_(generateId()) {
} // namespace neocpp
Request::Request(const std::string& method, const nlohmann::json& params)
    : jsonrpc_("2.0"), method_(method), params_(params), id_(generateId()) {
} // namespace neocpp
nlohmann::json Request::toJson() const {
    nlohmann::json json;
    json["jsonrpc"] = jsonrpc_;
    json["method"] = method_;
    json["params"] = params_;
    json["id"] = id_;
    return json;
} // namespace neocpp
std::string Request::toString() const {
    return toJson().dump();
} // namespace neocpp
SharedPtr<Request> Request::fromJson(const nlohmann::json& json) {
    auto request = std::make_shared<Request>();

    if (json.contains("jsonrpc")) {
        request->jsonrpc_ = json["jsonrpc"].get<std::string>();
    }

    if (json.contains("method")) {
        request->method_ = json["method"].get<std::string>();
    }

    if (json.contains("params")) {
        request->params_ = json["params"];
    }

    if (json.contains("id")) {
        request->id_ = json["id"].get<int>();
    }

    return request;
} // namespace neocpp
int Request::generateId() {
    return nextId_++;
} // namespace neocpp
} // namespace neocpp
