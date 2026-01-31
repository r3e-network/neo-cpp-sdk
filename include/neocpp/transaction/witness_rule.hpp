#pragma once

#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "neocpp/types/types.hpp"
#include "neocpp/types/hash160.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// Rule action for witness
enum class WitnessRuleAction : uint8_t {
    DENY = 0x00,
    ALLOW = 0x01
};

/// Condition type for witness rule
enum class WitnessConditionType : uint8_t {
    BOOLEAN = 0x00,
    NOT = 0x01,
    AND = 0x02,
    OR = 0x03,
    SCRIPT_HASH = 0x18,
    GROUP = 0x19,
    CALLED_BY_ENTRY = 0x20,
    CALLED_BY_CONTRACT = 0x28,
    CALLED_BY_GROUP = 0x29
};

/// Represents a witness rule condition
class WitnessCondition : public NeoSerializable {
private:
    WitnessConditionType type_;
    bool boolValue_;
    Hash160 scriptHash_;
    std::vector<SharedPtr<WitnessCondition>> conditions_;

public:
    /// Construct a boolean condition
    /// @param value The boolean value
    static SharedPtr<WitnessCondition> boolean(bool value);

    /// Construct a NOT condition
    /// @param condition The condition to negate
    static SharedPtr<WitnessCondition> notCondition(const SharedPtr<WitnessCondition>& condition);

    /// Construct an AND condition
    /// @param conditions The conditions to AND
    static SharedPtr<WitnessCondition> andCondition(const std::vector<SharedPtr<WitnessCondition>>& conditions);

    /// Construct an OR condition
    /// @param conditions The conditions to OR
    static SharedPtr<WitnessCondition> orCondition(const std::vector<SharedPtr<WitnessCondition>>& conditions);

    /// Construct a script hash condition
    /// @param scriptHash The script hash
    static SharedPtr<WitnessCondition> scriptHash(const Hash160& scriptHash);

    /// Construct a called-by-entry condition
    static SharedPtr<WitnessCondition> calledByEntry();

    /// Get the condition type
    /// @return The type
    [[nodiscard]] WitnessConditionType getType() const { return type_; }

    // NeoSerializable implementation
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static SharedPtr<WitnessCondition> deserialize(BinaryReader& reader);

private:
    WitnessCondition(WitnessConditionType type) : type_(type), boolValue_(false) {}
};

/// Represents a witness rule
class WitnessRule : public NeoSerializable {
public:
    /// Action type alias for compatibility
    using Action = WitnessRuleAction;

private:
    WitnessRuleAction action_;
    SharedPtr<WitnessCondition> condition_;

public:
    /// Default constructor
    WitnessRule() : action_(WitnessRuleAction::DENY) {}

    /// Construct with action and condition
    /// @param action The rule action
    /// @param condition The rule condition
    WitnessRule(WitnessRuleAction action, const SharedPtr<WitnessCondition>& condition)
        : action_(action), condition_(condition) {}

    /// Get the action
    /// @return The action
    [[nodiscard]] WitnessRuleAction getAction() const { return action_; }

    /// Set the action
    /// @param action The action
    void setAction(WitnessRuleAction action) { action_ = action; }

    /// Get the condition
    /// @return The condition
    const SharedPtr<WitnessCondition>& getCondition() const { return condition_; }

    /// Set the condition
    /// @param condition The condition
    void setCondition(const SharedPtr<WitnessCondition>& condition) { condition_ = condition; }

    // NeoSerializable implementation
    [[nodiscard]] size_t getSize() const override;
    void serialize(BinaryWriter& writer) const override;
    static SharedPtr<WitnessRule> deserialize(BinaryReader& reader);

    /// Convert to JSON
    nlohmann::json toJson() const;
};

} // namespace neocpp
