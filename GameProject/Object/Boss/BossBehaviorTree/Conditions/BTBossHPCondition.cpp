#include "BTBossHPCondition.h"
#include "../../Boss.h"

#ifdef _DEBUG
#include <imgui.h>
#endif

// ボスの最大HP（Boss.hのkMaxHp_と同じ値）
static const float kMaxHp = 200.0f;

BTBossHPCondition::BTBossHPCondition() {
    name_ = "HPCondition";
}

BTNodeStatus BTBossHPCondition::Execute(BTBlackboard* blackboard) {
    // ボスを取得
    Boss* boss = blackboard->GetBoss();
    if (!boss) {
        status_ = BTNodeStatus::Failure;
        return BTNodeStatus::Failure;
    }

    // 現在のHPをパーセンテージに変換
    float currentHp = boss->GetHp();
    float currentPercent = (currentHp / kMaxHp) * 100.0f;

    if (EvaluateCondition(currentPercent)) {
        status_ = BTNodeStatus::Success;
        return BTNodeStatus::Success;
    }

    status_ = BTNodeStatus::Failure;
    return BTNodeStatus::Failure;
}

void BTBossHPCondition::Reset() {
    BTNode::Reset();
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

void BTBossHPCondition::ApplyParameters(const nlohmann::json& params) {
    if (params.contains("thresholdPercent")) {
        thresholdPercent_ = params["thresholdPercent"].get<float>();
    }
    if (params.contains("comparison")) {
        comparison_ = static_cast<Comparison>(params["comparison"].get<int>());
    }
}

nlohmann::json BTBossHPCondition::ExtractParameters() const {
    return {
        {"thresholdPercent", thresholdPercent_},
        {"comparison", static_cast<int>(comparison_)}
    };
}

#ifdef _DEBUG
bool BTBossHPCondition::DrawImGui() {
    bool changed = false;

    // HP閾値の編集（パーセンテージ）
    if (ImGui::DragFloat("HP Threshold %##hp", &thresholdPercent_, 1.0f, 0.0f, 100.0f, "%.1f%%")) {
        changed = true;
    }

    // 比較タイプの選択
    int comp = static_cast<int>(comparison_);
    const char* compItems[] = { "Less (<)", "Less Or Equal (<=)", "Greater (>)", "Greater Or Equal (>=)" };
    if (ImGui::Combo("Comparison##hp", &comp, compItems, IM_ARRAYSIZE(compItems))) {
        comparison_ = static_cast<Comparison>(comp);
        changed = true;
    }

    return changed;
}
#endif
