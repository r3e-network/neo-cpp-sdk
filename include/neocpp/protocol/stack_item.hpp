#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <map>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/exceptions.hpp"

namespace neocpp {

// Forward declarations
class StackItem;
using StackItemPtr = std::shared_ptr<StackItem>;

/// Stack item types in Neo VM
enum class StackItemType : uint8_t {
    ANY = 0x00,
    POINTER = 0x10,
    BOOLEAN = 0x20,
    INTEGER = 0x21,
    BYTE_STRING = 0x28,
    BUFFER = 0x30,
    ARRAY = 0x40,
    STRUCT = 0x41,
    MAP = 0x48,
    INTEROP_INTERFACE = 0x60
};

/// Base class for Neo VM stack items
class StackItem {
public:
    virtual ~StackItem() = default;

    /// Get the type of this stack item
    virtual StackItemType getType() const = 0;

    /// Convert to JSON representation
    virtual nlohmann::json toJson() const = 0;

    /// Get as boolean
    virtual bool getBoolean() const {
        throw IllegalStateException("Cannot convert to boolean");
    }

    /// Get as integer
    virtual int64_t getInteger() const {
        throw IllegalStateException("Cannot convert to integer");
    }

    /// Get as byte array
    virtual Bytes getByteArray() const {
        throw IllegalStateException("Cannot convert to byte array");
    }

    /// Get as string
    virtual std::string getString() const {
        throw IllegalStateException("Cannot convert to string");
    }

    /// Get as array
    virtual std::vector<StackItemPtr> getArray() const {
        throw IllegalStateException("Cannot convert to array");
    }

    /// Get as map
    virtual std::map<StackItemPtr, StackItemPtr, std::owner_less<StackItemPtr>> getMap() const {
        throw IllegalStateException("Cannot convert to map");
    }

    /// Create from JSON
    static StackItemPtr fromJson(const nlohmann::json& json);

protected:
    StackItem() = default;
};

/// Boolean stack item
class BooleanStackItem : public StackItem {
private:
    bool value_;

public:
    explicit BooleanStackItem(bool value) : value_(value) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::BOOLEAN;
    }

    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"type", "Boolean"},
            {"value", value_}
        };
    }
    [[nodiscard]] bool getBoolean() const override { return value_; }
    [[nodiscard]] int64_t getInteger() const override {
        return value_ ? 1 : 0;
    }
};

/// Integer stack item
class IntegerStackItem : public StackItem {
private:
    int64_t value_;

public:
    explicit IntegerStackItem(int64_t value) : value_(value) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::INTEGER;
    }

    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"type", "Integer"},
            {"value", std::to_string(value_)}
        };
    }
    [[nodiscard]] bool getBoolean() const override {
        return value_ != 0;
    }
    [[nodiscard]] int64_t getInteger() const override { return value_; }
};

/// Byte string stack item
class ByteStringStackItem : public StackItem {
private:
    Bytes value_;

public:
    explicit ByteStringStackItem(const Bytes& value) : value_(value) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::BYTE_STRING;
    }

    nlohmann::json toJson() const override;
    [[nodiscard]] bool getBoolean() const override {
        return !value_.empty();
    }
    [[nodiscard]] Bytes getByteArray() const override { return value_; }

    std::string getString() const override;
};

/// Array stack item
class ArrayStackItem : public StackItem {
protected:
    std::vector<StackItemPtr> items_;

public:
    explicit ArrayStackItem(const std::vector<StackItemPtr>& items) : items_(items) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::ARRAY;
    }

    nlohmann::json toJson() const override;
    [[nodiscard]] bool getBoolean() const override {
        return !items_.empty();
    }

    std::vector<StackItemPtr> getArray() const override { return items_; }

    size_t size() const { return items_.size(); }

    StackItemPtr at(size_t index) const {
        if (index >= items_.size()) {
            throw IllegalArgumentException("Index out of bounds");
        }
        return items_[index];
    }
};

/// Struct stack item (similar to array but different type)
class StructStackItem : public ArrayStackItem {
public:
    explicit StructStackItem(const std::vector<StackItemPtr>& items)
        : ArrayStackItem(items) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::STRUCT;
    }
};

/// Map stack item
class MapStackItem : public StackItem {
private:
    std::map<StackItemPtr, StackItemPtr, std::owner_less<StackItemPtr>> map_;

public:
    explicit MapStackItem(const std::map<StackItemPtr, StackItemPtr, std::owner_less<StackItemPtr>>& map) : map_(map) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::MAP;
    }

    nlohmann::json toJson() const override;
    [[nodiscard]] bool getBoolean() const override {
        return !map_.empty();
    }

    std::map<StackItemPtr, StackItemPtr, std::owner_less<StackItemPtr>> getMap() const override {
        return map_;
    }

    size_t size() const { return map_.size(); }
};

/// Pointer stack item
class PointerStackItem : public StackItem {
private:
    int64_t value_;

public:
    explicit PointerStackItem(int64_t value) : value_(value) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::POINTER;
    }

    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"type", "Pointer"},
            {"value", value_}
        };
    }
    [[nodiscard]] int64_t getInteger() const override { return value_; }
};

/// InteropInterface stack item
class InteropInterfaceStackItem : public StackItem {
private:
    std::string id_;

public:
    explicit InteropInterfaceStackItem(const std::string& id) : id_(id) {}
    [[nodiscard]] StackItemType getType() const override {
        return StackItemType::INTEROP_INTERFACE;
    }

    nlohmann::json toJson() const override {
        return nlohmann::json{
            {"type", "InteropInterface"},
            {"interface", id_}
        };
    }

    std::string getString() const override { return id_; }
};


} // namespace neocpp
