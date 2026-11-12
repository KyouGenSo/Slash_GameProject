#include "BTBossHPCondition.h"
#include "Script/Object/Boss/Boss.h"

BTBossHPCondition::BTBossHPCondition() {
    name_ = "HPCondition";
}

Tako::BTNodeStatus BTBossHPCondition::Execute(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    if (!boss) {
        status_ = Tako::BTNodeStatus::Failure;
        return Tako::BTNodeStatus::Failure;
    }

    float currentHp = boss->GetHp();
    float currentPercent = (currentHp / Boss::GetMaxHp()) * 100.0f;

    if (EvaluateCondition(currentPercent)) {
        status_ = Tako::BTNodeStatus::Success;
        return Tako::BTNodeStatus::Success;
    }

    status_ = Tako::BTNodeStatus::Failure;
    return Tako::BTNodeStatus::Failure;
}

void BTBossHPCondition::Reset() {
    Tako::BTNode::Reset();
}

bool BTBossHPCondition::EvaluateCondition(float currentPercent) const {
    switch (comparison_) {
    case Comparison::Less:
        return currentPercent < thresholdPercent_;
    case Comparison::LessOrEqual:
        return currentPercent <= thresholdPercent_;
    case Comparison::Greater:
        return currentPercent > thresholdPercent_;
    case Comparison::GreaterOrEqual:
        return currentPercent >= thresholdPercent_;
    default:
        return false;
    }
}

