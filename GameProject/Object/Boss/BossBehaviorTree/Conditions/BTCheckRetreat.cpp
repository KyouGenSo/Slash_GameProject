#include "BTCheckRetreat.h"
#include "../../Boss.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

BTCheckRetreat::BTCheckRetreat() {
    name_ = "CheckRetreat";
}

BTNodeStatus BTCheckRetreat::Execute(BTBlackboard* blackboard) {
    // ボスを取得
    Boss* boss = blackboard->GetBoss();
    if (!boss) {
        status_ = BTNodeStatus::Failure;
        return BTNodeStatus::Failure;
    }

    // 離脱要求をチェック
    if (boss->ShouldRetreat()) {
        status_ = BTNodeStatus::Success;
        return BTNodeStatus::Success;
    }

    status_ = BTNodeStatus::Failure;
    return BTNodeStatus::Failure;
}

void BTCheckRetreat::Reset() {
    BTNode::Reset();
}

#ifdef _DEBUG
bool BTCheckRetreat::DrawImGui() {
    ImGui::Text("Checks if boss should retreat.");
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No parameters.");
    return false;
}
#endif
