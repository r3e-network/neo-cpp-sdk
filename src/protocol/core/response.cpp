#include "neocpp/protocol/core/response.hpp"

namespace neocpp {

Response::Response() : jsonrpc_("2.0"), id_(0) {
} // namespace neocpp
Response::Response(const nlohmann::json& result)
    : jsonrpc_("2.0"), result_(result), id_(0) {
} // namespace neocpp
Response::Response(const Error& error)
    : jsonrpc_("2.0"), error_(std::make_unique<Error>(error)), id_(0) {
} // namespace neocpp
nlohmann::json Response::toJson() const {
    nlohmann::json json;
    json["jsonrpc"] = jsonrpc_;
    json["id"] = id_;

    if (error_) {
        nlohmann::json errorJson;
        errorJson["code"] = error_->code;
        errorJson["message"] = error_->message;
        if (!error_->data.is_null()) {
            errorJson["data"] = error_->data;
        }
        json["error"] = errorJson;
    } else {
        json["result"] = result_;
    }

    return json;
} // namespace neocpp
std::string Response::toString() const {
    return toJson().dump();
} // namespace neocpp
SharedPtr<Response> Response::fromJson(const nlohmann::json& json) {
    auto response = std::make_shared<Response>();

    if (json.contains("jsonrpc")) {
        response->jsonrpc_ = json["jsonrpc"].get<std::string>();
    }

    if (json.contains("id")) {
        if (json["id"].is_number()) {
            response->id_ = json["id"].get<int>();
        }
    }

    if (json.contains("error")) {
        Error error;
        const auto& errorJson = json["error"];

        if (errorJson.contains("code")) {
            error.code = errorJson["code"].get<int>();
        }

        if (errorJson.contains("message")) {
            error.message = errorJson["message"].get<std::string>();
        }

        if (errorJson.contains("data")) {
            error.data = errorJson["data"];
        }

        response->error_ = std::make_unique<Error>(error);
    } else if (json.contains("result")) {
        response->result_ = json["result"];
    }

    return response;
} // namespace neocpp
} // namespace neocpp
