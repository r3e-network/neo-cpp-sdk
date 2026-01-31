#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"

namespace neocpp {

/// Witness condition types
enum class WitnessConditionType : uint8_t {
    /// Boolean condition
    BOOLEAN = 0x00,

    /// Not condition
    NOT = 0x01,

    /// And condition
    AND = 0x02,

    /// Or condition
    OR = 0x03,

    /// Script hash condition
    SCRIPT_HASH = 0x18,

    /// Group condition
    GROUP = 0x19,

    /// Called by entry condition
    CALLED_BY_ENTRY = 0x20,

    /// Called by contract condition
    CALLED_BY_CONTRACT = 0x28,

    /// Called by group condition
    CALLED_BY_GROUP = 0x29
};

// Forward declaration
class BinaryWriter;
class BinaryReader;

/// Base witness condition
class WitnessCondition {
protected:
    WitnessConditionType type_;

public:
    /// Constructor
    explicit WitnessCondition(WitnessConditionType type) : type_(type) {}

    /// Destructor
    virtual ~WitnessCondition() = default;

    /// Get type
    [[nodiscard]] WitnessConditionType getType() const { return type_; }

    /// Get size
    virtual size_t getSize() const = 0;

    /// Serialize
    virtual void serialize(BinaryWriter& writer) const = 0;

    /// Deserialize
    static SharedPtr<WitnessCondition> deserialize(BinaryReader& reader);

    /// Convert to JSON
    virtual nlohmann::json toJson() const = 0;
};

/// Boolean witness condition
class BooleanCondition : public WitnessCondition {
private:
    bool value_;

public:
    /// Constructor
    explicit BooleanCondition(bool value)
        : WitnessCondition(WitnessConditionType::BOOLEAN), value_(value) {}

    /// Get value
    [[nodiscard]] bool getValue() const { return value_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Not witness condition
class NotCondition : public WitnessCondition {
private:
    SharedPtr<WitnessCondition> expression_;

public:
    /// Constructor
    explicit NotCondition(const SharedPtr<WitnessCondition>& expression)
        : WitnessCondition(WitnessConditionType::NOT), expression_(expression) {}

    /// Get expression
    const SharedPtr<WitnessCondition>& getExpression() const { return expression_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// And witness condition
class AndCondition : public WitnessCondition {
private:
    std::vector<SharedPtr<WitnessCondition>> expressions_;

public:
    /// Constructor
    explicit AndCondition(const std::vector<SharedPtr<WitnessCondition>>& expressions)
        : WitnessCondition(WitnessConditionType::AND), expressions_(expressions) {}

    /// Get expressions
    const std::vector<SharedPtr<WitnessCondition>>& getExpressions() const { return expressions_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Or witness condition
class OrCondition : public WitnessCondition {
private:
    std::vector<SharedPtr<WitnessCondition>> expressions_;

public:
    /// Constructor
    explicit OrCondition(const std::vector<SharedPtr<WitnessCondition>>& expressions)
        : WitnessCondition(WitnessConditionType::OR), expressions_(expressions) {}

    /// Get expressions
    const std::vector<SharedPtr<WitnessCondition>>& getExpressions() const { return expressions_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Script hash witness condition
class ScriptHashCondition : public WitnessCondition {
private:
    Hash160 scriptHash_;

public:
    /// Constructor
    explicit ScriptHashCondition(const Hash160& scriptHash)
        : WitnessCondition(WitnessConditionType::SCRIPT_HASH), scriptHash_(scriptHash) {}

    /// Get script hash
    const Hash160& getScriptHash() const { return scriptHash_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Group witness condition
class GroupCondition : public WitnessCondition {
private:
    Bytes pubKey_;

public:
    /// Constructor
    explicit GroupCondition(const Bytes& pubKey)
        : WitnessCondition(WitnessConditionType::GROUP), pubKey_(pubKey) {}

    /// Get public key
    const Bytes& getPubKey() const { return pubKey_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Called by entry witness condition
class CalledByEntryCondition : public WitnessCondition {
public:
    /// Constructor
    CalledByEntryCondition()
        : WitnessCondition(WitnessConditionType::CALLED_BY_ENTRY) {}
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Called by contract witness condition
class CalledByContractCondition : public WitnessCondition {
private:
    Hash160 scriptHash_;

public:
    /// Constructor
    explicit CalledByContractCondition(const Hash160& scriptHash)
        : WitnessCondition(WitnessConditionType::CALLED_BY_CONTRACT), scriptHash_(scriptHash) {}

    /// Get script hash
    const Hash160& getScriptHash() const { return scriptHash_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

/// Called by group witness condition
class CalledByGroupCondition : public WitnessCondition {
private:
    Bytes pubKey_;

public:
    /// Constructor
    explicit CalledByGroupCondition(const Bytes& pubKey)
        : WitnessCondition(WitnessConditionType::CALLED_BY_GROUP), pubKey_(pubKey) {}

    /// Get public key
    const Bytes& getPubKey() const { return pubKey_; }
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    nlohmann::json toJson() const override;
};

} // namespace neocpp
