#include "BTBossDistanceCondition.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include "Vector3.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

BTBossDistanceCondition::BTBossDistanceCondition() {
    name_ = "DistanceCondition";
}

Tako::BTNodeStatus BTBossDistanceCondition::Execute(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    Player* player = blackboard->GetPtr<Player>("player");

    if (!boss || !player) {
        status_ = Tako::BTNodeStatus::Failure;
        return Tako::BTNodeStatus::Failure;
    }

    Vector3 bossPos = boss->GetTransform().translate;
    Vector3 playerPos = player->GetTransform().translate;

    // 水平距離（Y 軸を無視）
    Vector3 diff = playerPos - bossPos;
    diff.y = 0.0f;
    float distance = diff.Length();

    if (distance >= minDistance_ && distance <= maxDistance_) {
        status_ = Tako::BTNodeStatus::Success;
        return Tako::BTNodeStatus::Success;
    }

    status_ = Tako::BTNodeStatus::Failure;
    return Tako::BTNodeStatus::Failure;
}

void BTBossDistanceCondition::Reset() {
    Tako::BTNode::Reset();
}

void BTBossDistanceCondition::PostEditChange() {
    maxDistance_ = (std::max)(maxDistance_, minDistance_);
}

#ifdef _DEBUG
bool BTBossDistanceCondition::DrawCustomImGui() {
    ImGui::TextDisabled("Success if: %.1f <= distance <= %.1f", minDistance_, maxDistance_);
    return false;
}
#endif
