#include "BTBossPhaseCondition.h"
#include "../../Boss.h"

BTBossPhaseCondition::BTBossPhaseCondition() {
    name_ = "PhaseCondition";
}

Tako::BTNodeStatus BTBossPhaseCondition::Execute(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    if (!boss) {
        status_ = Tako::BTNodeStatus::Failure;
        return Tako::BTNodeStatus::Failure;
    }

    uint32_t currentPhase = boss->GetPhase();

    if (EvaluateCondition(currentPhase)) {
        status_ = Tako::BTNodeStatus::Success;
        return Tako::BTNodeStatus::Success;
    }

    status_ = Tako::BTNodeStatus::Failure;
    return Tako::BTNodeStatus::Failure;
}

void BTBossPhaseCondition::Reset() {
    Tako::BTNode::Reset();
}

bool BTBossPhaseCondition::EvaluateCondition(uint32_t currentPhase) const {
    switch (comparison_) {
    case Comparison::Equal:
        return currentPhase == targetPhase_;
    case Comparison::NotEqual:
        return currentPhase != targetPhase_;
    case Comparison::GreaterOrEqual:
        return currentPhase >= targetPhase_;
    case Comparison::LessOrEqual:
        return currentPhase <= targetPhase_;
    default:
        return false;
    }
}

