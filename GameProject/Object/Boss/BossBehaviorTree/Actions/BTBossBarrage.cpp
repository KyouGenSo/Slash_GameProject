#include "BTBossBarrage.h"
#include "../../Boss.h"
#include "../../../Player/Player.h"
#include "../../../../Common/GameConst.h"
#include "RandomEngine.h"

#include <cmath>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

BTBossBarrage::BTBossBarrage() {
    name_ = "BossBarrage";
}

BTNodeStatus BTBossBarrage::Execute(BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetBoss();
    if (!boss) {
        status_ = BTNodeStatus::Failure;
        return BTNodeStatus::Failure;
    }

    float deltaTime = blackboard->GetDeltaTime();

    // 初回実行時の初期化
    if (isFirstExecute_) {
        InitializeBarrage(boss);
        isFirstExecute_ = false;
    }

    // フェーズ管理: Move → Charge → Firing → Recovery
    float moveEnd = moveDuration_;
    float chargeEnd = moveEnd + chargeTime_;
    float firingEnd = chargeEnd + firingDuration_;

    // Phase: Move（ステージ中央への移動）
    if (elapsedTime_ < moveEnd) {
        UpdateMove(boss, deltaTime);
    }
    // Phase: Charge（射撃予兆）
    else if (elapsedTime_ < chargeEnd) {
        bulletSignEffect_.Update(boss, deltaTime);
    }
    // Phase: Firing（弾幕発射）
    else if (elapsedTime_ < firingEnd) {
        // エフェクト終了
        if (!hasEndedEffect_) {
            bulletSignEffect_.End(boss);
            hasEndedEffect_ = true;
        }

        // 発射間隔チェック
        timeSinceLastFire_ += deltaTime;
        if (timeSinceLastFire_ >= fireInterval_) {
            FireRandomBullet(boss);
            timeSinceLastFire_ = 0.0f;
        }
    }
    // Phase: Recovery（硬直）
    else if (!enteredRecovery_) {
        boss->EnterRecovery();
        enteredRecovery_ = true;
    }

    // 経過時間を更新
    elapsedTime_ += deltaTime;

    // 状態終了チェック
    if (elapsedTime_ >= totalDuration_) {
        // 硬直フェーズ終了
        boss->ExitRecovery();

        // リセットして成功を返す
        isFirstExecute_ = true;
        elapsedTime_ = 0.0f;
        timeSinceLastFire_ = 0.0f;
        hasEndedEffect_ = false;
        enteredRecovery_ = false;
        status_ = BTNodeStatus::Success;
        return BTNodeStatus::Success;
    }

    // まだ処理中
    status_ = BTNodeStatus::Running;
    return BTNodeStatus::Running;
}

void BTBossBarrage::Reset() {
    BTNode::Reset();
    elapsedTime_ = 0.0f;
    timeSinceLastFire_ = 0.0f;
    isFirstExecute_ = true;
    hasEndedEffect_ = false;
    enteredRecovery_ = false;
}

void BTBossBarrage::InitializeBarrage(Boss* boss) {
    // タイマーリセット
    elapsedTime_ = 0.0f;
    timeSinceLastFire_ = 0.0f;
    hasEndedEffect_ = false;
    enteredRecovery_ = false;

    // totalDurationを計算
    totalDuration_ = moveDuration_ + chargeTime_ + firingDuration_ + recoveryTime_;

    // 開始位置を記録
    startPosition_ = boss->GetTransform().translate;

    // 目標位置をGameConstから計算（ステージ中央）
    float targetX = (GameConst::kStageXMin + GameConst::kStageXMax) / 2.0f;
    float targetZ = (GameConst::kStageZMin + GameConst::kStageZMax) / 2.0f;
    targetPosition_ = Vector3(targetX, startPosition_.y, targetZ);

    // 射撃予兆エフェクト開始（チャージフェーズ開始時に表示されるよう準備）
    bulletSignEffect_.Start(boss, chargeTime_);
}

void BTBossBarrage::UpdateMove(Boss* boss, float deltaTime) {
    deltaTime; // 未使用警告抑制

    if (elapsedTime_ < moveDuration_) {
        // 移動中
        float t = elapsedTime_ / moveDuration_;

        // イージング（加速→減速）
        t = t * t * (kEasingCoeffA - kEasingCoeffB * t);

        Vector3 newPosition = Vector3::Lerp(startPosition_, targetPosition_, t);
        boss->SetTranslate(newPosition);

        // 目標方向を向く
        Vector3 direction = targetPosition_ - startPosition_;
        direction.y = 0.0f;
        if (direction.Length() > kDirectionEpsilon) {
            direction = direction.Normalize();
            float angle = atan2f(direction.x, direction.z);
            boss->SetRotate(Vector3(0.0f, angle, 0.0f));
        }
    } else {
        // 移動完了、最終位置に設定
        boss->SetTranslate(targetPosition_);
    }
}

void BTBossBarrage::FireRandomBullet(Boss* boss) {
    // 発射位置
    Vector3 firePosition = boss->GetTransform().translate;

    // ランダムな方向（XZ平面）
    RandomEngine* rng = RandomEngine::GetInstance();
    Vector3 direction = rng->GetRandomDirectionXZ();

    // 弾種を確率で決定
    bool isPenetrating = rng->GetBool(penetratingRatio_);

    float speed;
    if (isPenetrating) {
        // 貫通弾（遅い）
        speed = rng->GetFloat(penetratingBulletSpeedMin_, penetratingBulletSpeedMax_);
        boss->RequestPenetratingBulletSpawn(firePosition, direction * speed);
    } else {
        // 通常弾（速い）
        speed = rng->GetFloat(normalBulletSpeedMin_, normalBulletSpeedMax_);
        boss->RequestBulletSpawn(firePosition, direction * speed);
    }
}

nlohmann::json BTBossBarrage::ExtractParameters() const {
    return {
        {"moveDuration", moveDuration_},
        {"chargeTime", chargeTime_},
        {"firingDuration", firingDuration_},
        {"recoveryTime", recoveryTime_},
        {"fireInterval", fireInterval_},
        {"normalBulletSpeedMin", normalBulletSpeedMin_},
        {"normalBulletSpeedMax", normalBulletSpeedMax_},
        {"penetratingBulletSpeedMin", penetratingBulletSpeedMin_},
        {"penetratingBulletSpeedMax", penetratingBulletSpeedMax_},
        {"penetratingRatio", penetratingRatio_}
    };
}

#ifdef _DEBUG
bool BTBossBarrage::DrawImGui() {
    bool changed = false;

    ImGui::SeparatorText("Phase Timing");
    if (ImGui::DragFloat("Move Duration##barrage", &moveDuration_, 0.05f, 0.1f, 2.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Charge Time##barrage", &chargeTime_, 0.05f, 0.0f, 3.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Firing Duration##barrage", &firingDuration_, 0.1f, 0.5f, 10.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Recovery Time##barrage", &recoveryTime_, 0.05f, 0.0f, 3.0f)) {
        changed = true;
    }

    ImGui::SeparatorText("Fire Control");
    if (ImGui::DragFloat("Fire Interval##barrage", &fireInterval_, 0.01f, 0.02f, 0.5f)) {
        changed = true;
    }
    // 発射レートを表示
    float fireRate = 1.0f / fireInterval_;
    ImGui::Text("Fire Rate: %.1f shots/sec", fireRate);

    ImGui::SeparatorText("Normal Bullet Speed");
    if (ImGui::DragFloat("Speed Min##normal", &normalBulletSpeedMin_, 1.0f, 5.0f, 100.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Speed Max##normal", &normalBulletSpeedMax_, 1.0f, 5.0f, 100.0f)) {
        changed = true;
    }

    ImGui::SeparatorText("Penetrating Bullet Speed");
    if (ImGui::DragFloat("Speed Min##penetrating", &penetratingBulletSpeedMin_, 1.0f, 5.0f, 50.0f)) {
        changed = true;
    }
    if (ImGui::DragFloat("Speed Max##penetrating", &penetratingBulletSpeedMax_, 1.0f, 5.0f, 50.0f)) {
        changed = true;
    }

    ImGui::SeparatorText("Bullet Type Ratio");
    if (ImGui::SliderFloat("Penetrating Ratio##barrage", &penetratingRatio_, 0.0f, 1.0f)) {
        changed = true;
    }

    return changed;
}
#endif
