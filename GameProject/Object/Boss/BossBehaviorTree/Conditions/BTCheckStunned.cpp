#include "BTCheckStunned.h"
#include "../../Boss.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

BTCheckStunned::BTCheckStunned() {
    name_ = "CheckStunned";
}

BTNodeStatus BTCheckStunned::Execute(BTBlackboard* blackboard) {
    // ボスを取得
    Boss* boss = blackboard->GetBoss();
    if (!boss) {
        status_ = BTNodeStatus::Failure;
        return BTNodeStatus::Failure;
    }

    // スタン状態をチェック
    if (boss->IsStunned()) {
        status_ = BTNodeStatus::Success;
        return BTNodeStatus::Success;
    }

    status_ = BTNodeStatus::Failure;
    return BTNodeStatus::Failure;
}

void BTCheckStunned::Reset() {
    BTNode::Reset();
}

#ifdef _DEBUG
bool BTCheckStunned::DrawImGui() {
    ImGui::Text("Checks if boss is currently stunned.");
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No parameters.");
    return false;
}
#endif
