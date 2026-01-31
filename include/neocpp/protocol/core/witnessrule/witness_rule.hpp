#pragma once

#include <memory>
#include "neocpp/types/types.hpp"
#include "neocpp/protocol/core/witnessrule/witness_action.hpp"
#include "neocpp/protocol/core/witnessrule/witness_condition.hpp"
#include "neocpp/serialization/neo_serializable.hpp"

namespace neocpp {

// Forward declarations
class BinaryWriter;
class BinaryReader;

/// Witness rule for transaction validation
class WitnessRule : public NeoSerializable {
private:
    WitnessAction action_;
    SharedPtr<WitnessCondition> condition_;

public:
    /// Constructor
    WitnessRule() : action_(WitnessAction::DENY) {}

    /// Constructor
    /// @param action The witness action
    /// @param condition The witness condition
    WitnessRule(WitnessAction action, const SharedPtr<WitnessCondition>& condition)
        : action_(action), condition_(condition) {}

    /// Get action
    [[nodiscard]] WitnessAction getAction() const { return action_; }

    /// Set action
    void setAction(WitnessAction action) { action_ = action; }

    /// Get condition
    const SharedPtr<WitnessCondition>& getCondition() const { return condition_; }

    /// Set condition
    void setCondition(const SharedPtr<WitnessCondition>& condition) { condition_ = condition; }

    /// Get size
    [[nodiscard]] size_t getSize() const override;

    /// Serialize
    void serialize(BinaryWriter& writer) const override;

    /// Deserialize
    static WitnessRule deserialize(BinaryReader& reader);

    /// Convert to JSON
    nlohmann::json toJson() const;

    /// Create from JSON
    static SharedPtr<WitnessRule> fromJson(const nlohmann::json& json);

    /// Create allow rule
    /// @param condition The condition
    /// @return The witness rule
    static SharedPtr<WitnessRule> allow(const SharedPtr<WitnessCondition>& condition) {
        return std::make_shared<WitnessRule>(WitnessAction::ALLOW, condition);
    }

    /// Create deny rule
    /// @param condition The condition
    /// @return The witness rule
    static SharedPtr<WitnessRule> deny(const SharedPtr<WitnessCondition>& condition) {
        return std::make_shared<WitnessRule>(WitnessAction::DENY, condition);
    }

private:
    /// Parse condition from JSON
    static SharedPtr<WitnessCondition> parseConditionFromJson(const nlohmann::json& json);
};

} // namespace neocpp
