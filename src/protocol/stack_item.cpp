
#include "neocpp/protocol/stack_item.hpp"
#include "neocpp/utils/hex.hpp"
#include <algorithm>

namespace neocpp {

// ByteStringStackItem implementation
nlohmann::json ByteStringStackItem::toJson() const {
    return nlohmann::json{
        {"type", "ByteString"},
        {"value", Hex::encode(value_)}
    };
} // namespace neocpp
std::string ByteStringStackItem::getString() const {
    // Try to interpret as UTF-8 string
    return std::string(value_.begin(), value_.end());
} // namespace neocpp
// ArrayStackItem implementation
nlohmann::json ArrayStackItem::toJson() const {
    nlohmann::json items = nlohmann::json::array();
    for (const auto& item : items_) {
        items.push_back(item->toJson());
    }
    return nlohmann::json{
        {"type", "Array"},
        {"value", items}
    };
} // namespace neocpp
// MapStackItem implementation
nlohmann::json MapStackItem::toJson() const {
    nlohmann::json entries = nlohmann::json::array();
    for (const auto& [key, value] : map_) {
        entries.push_back(nlohmann::json{
            {"key", key->toJson()},
            {"value", value->toJson()}
        });
    }
    return nlohmann::json{
        {"type", "Map"},
        {"value", entries}
    };
} // namespace neocpp
// Static factory method
StackItemPtr StackItem::fromJson(const nlohmann::json& json) {
    if (!json.contains("type")) {
        throw DeserializationException("Stack item JSON must contain 'type' field");
    }

    std::string type = json["type"].get<std::string>();

    if (type == "Boolean") {
        return std::make_shared<BooleanStackItem>(json["value"].get<bool>());
    }
    else if (type == "Integer") {
        std::string valueStr = json["value"].get<std::string>();
        int64_t value = std::stoll(valueStr);
        return std::make_shared<IntegerStackItem>(value);
    }
    else if (type == "ByteString") {
        std::string hexStr = json["value"].get<std::string>();
        Bytes bytes = Hex::decode(hexStr);
        return std::make_shared<ByteStringStackItem>(bytes);
    }
    else if (type == "Array") {
        std::vector<StackItemPtr> items;
        for (const auto& item : json["value"]) {
            items.push_back(fromJson(item));
        }
        return std::make_shared<ArrayStackItem>(items);
    }
    else if (type == "Struct") {
        std::vector<StackItemPtr> items;
        for (const auto& item : json["value"]) {
            items.push_back(fromJson(item));
        }
        return std::make_shared<StructStackItem>(items);
    }
    else if (type == "Map") {
        // Parse map entries using owner_less comparator for shared_ptr keys
        std::map<StackItemPtr, StackItemPtr, std::owner_less<StackItemPtr>> map;
        if (json.contains("value") && json["value"].is_array()) {
            for (const auto& entry : json["value"]) {
                if (entry.contains("key") && entry.contains("value")) {
                    auto key = fromJson(entry["key"]);
                    auto value = fromJson(entry["value"]);
                    map[key] = value;
                }
            }
        }
        return std::make_shared<MapStackItem>(map);
    }
    else if (type == "Pointer") {
        return std::make_shared<PointerStackItem>(json["value"].get<int64_t>());
    }
    else if (type == "InteropInterface") {
        return std::make_shared<InteropInterfaceStackItem>(json["interface"].get<std::string>());
    }
    else {
        throw DeserializationException("Unknown stack item type: " + type);
    }
} // namespace neocpp
} // namespace neocpp
