#include "BTBossStun.h"
#include "../../Boss.h"
#include "../../../../Common/GameConst.h"

#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

BTBossStun::BTBossStun() {
    name_ = "BossStun";
}

BTNodeStatus BTBossStun::Execute(BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetBoss();
    if (!boss) {
        status_ = BTNodeStatus::Failure;
        return BTNodeStatus::Failure;
    }

    float deltaTime = blackboard->GetDeltaTime();

    // 初回実行時の初期化
    if (isFirstExecute_) {
        InitializeStun(boss);
        isFirstExecute_ = false;
    }

    elapsedTime_ += deltaTime;

    // ノックバック処理（スタン開始直後のみ）
    if (!knockbackComplete_) {
        UpdateKnockback(boss);
    }

    // 色点滅処理（スタン中継続）
    flashTimer_ += deltaTime;
    if (flashTimer_ >= flashInterval_) {
        flashTimer_ = 0.0f;
        UpdateFlash(boss);
    }

    // スタン終了判定
    if (elapsedTime_ >= stunDuration_) {
        boss->ClearStun();

        // リセットして成功を返す
        isFirstExecute_ = true;
        elapsedTime_ = 0.0f;
        flashTimer_ = 0.0f;
        knockbackComplete_ = false;
        status_ = BTNodeStatus::Success;
        return BTNodeStatus::Success;
    }

    // まだスタン中
    status_ = BTNodeStatus::Running;
    return BTNodeStatus::Running;
}

void BTBossStun::Reset() {
    BTNode::Reset();
    elapsedTime_ = 0.0f;
    flashTimer_ = 0.0f;
    isFirstExecute_ = true;
    knockbackComplete_ = false;
}

void BTBossStun::InitializeStun(Boss* boss) {
    elapsedTime_ = 0.0f;
    flashTimer_ = 0.0f;
    knockbackComplete_ = false;

    startPosition_ = boss->GetTransform().translate;

    // ノックバック方向を取得
    Vector3 knockbackDir = boss->GetStunKnockbackDirection();

    // 方向が有効かチェック
    if (knockbackDir.Length() < kDirectionEpsilon) {
        // フォールバック: ボスの後方に下がる
        float angle = boss->GetTransform().rotate.y;
        knockbackDir = Vector3(-sinf(angle), 0.0f, -cosf(angle));
    }

    knockbackDir.y = 0.0f;
    knockbackDir = knockbackDir.Normalize();

    // 目標位置を計算
    targetPosition_ = startPosition_ + knockbackDir * knockbackDistance_;
    targetPosition_ = ClampToArea(targetPosition_);

    // 初回フラッシュを即座に開始
    UpdateFlash(boss);
}

void BTBossStun::UpdateKnockback(Boss* boss) {
    if (elapsedTime_ >= knockbackDuration_) {
        knockbackComplete_ = true;
        boss->SetTranslate(targetPosition_);
        return;
    }

    float t = elapsedTime_ / knockbackDuration_;
    t = std::clamp(t, 0.0f, 1.0f);

    // イージング（加速→減速）: smoothstep
    t = t * t * (kEasingCoeffA - kEasingCoeffB * t);

    Vector3 newPosition = Vector3::Lerp(startPosition_, targetPosition_, t);
    boss->SetTranslate(newPosition);
}

void BTBossStun::UpdateFlash(Boss* boss) {
    boss->StartStunFlash(stunFlashColor_, flashDuration_);
}

Vector3 BTBossStun::ClampToArea(const Vector3& position) {
    Vector3 clampedPos = position;

    // GameConstantsのステージ境界を使用
    clampedPos.x = std::clamp(clampedPos.x,
        GameConst::kStageXMin + GameConst::kAreaMargin,
        GameConst::kStageXMax - GameConst::kAreaMargin);

    clampedPos.z = std::clamp(clampedPos.z,
        GameConst::kStageZMin + GameConst::kAreaMargin,
        GameConst::kStageZMax - GameConst::kAreaMargin);

    // Y座標は元の値を保持
    clampedPos.y = position.y;

    return clampedPos;
}

void BTBossStun::ApplyParameters(const nlohmann::json& params) {
    if (params.contains("stunDuration")) {
        stunDuration_ = params["stunDuration"];
    }
    if (params.contains("knockbackDistance")) {
        knockbackDistance_ = params["knockbackDistance"];
    }
    if (params.contains("knockbackDuration")) {
        knockbackDuration_ = params["knockbackDuration"];
    }
    if (params.contains("flashInterval")) {
        flashInterval_ = params["flashInterval"];
    }
    if (params.contains("flashDuration")) {
        flashDuration_ = params["flashDuration"];
    }
}

nlohmann::json BTBossStun::ExtractParameters() const {
    return {
        {"stunDuration", stunDuration_},
        {"knockbackDistance", knockbackDistance_},
        {"knockbackDuration", knockbackDuration_},
        {"flashInterval", flashInterval_},
        {"flashDuration", flashDuration_}
    };
}

#ifdef _DEBUG
bool BTBossStun::DrawImGui() {
    bool changed = false;

    ImGui::Text("Stun Parameters:");
    if (ImGui::DragFloat("Stun Duration##stun", &stunDuration_, 0.1f, 0.1f, 5.0f, "%.1f sec")) {
        changed = true;
    }

    ImGui::Separator();
    ImGui::Text("Knockback:");
    if (ImGui::DragFloat("Distance##stun", &knockbackDistance_, 0.5f, 0.0f, 30.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Duration##knockback", &knockbackDuration_, 0.05f, 0.1f, 1.0f, "%.2f sec")) {
        changed = true;
    }

    ImGui::Separator();
    ImGui::Text("Flash Effect:");
    if (ImGui::DragFloat("Interval##flash", &flashInterval_, 0.01f, 0.01f, 0.5f, "%.2f sec")) {
        changed = true;
    }
    if (ImGui::DragFloat("Flash Duration##flash", &flashDuration_, 0.01f, 0.01f, 0.2f, "%.2f sec")) {
        changed = true;
    }

    ImGui::ColorEdit4("Flash Color##stun", &stunFlashColor_.x);

    ImGui::Separator();
    ImGui::Text("Runtime Info:");
    ImGui::Text("Elapsed: %.2f / %.2f sec", elapsedTime_, stunDuration_);
    ImGui::Text("Knockback: %s", knockbackComplete_ ? "Complete" : "In Progress");

    return changed;
}
#endif
