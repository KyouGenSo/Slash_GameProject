#include "BTBossRingShockwave.h"
#include "../../Boss.h"

#include "Object3d.h"
#include "EmitterManager.h"

#include <algorithm>
#include <cmath>

using namespace Tako;

BTBossRingShockwave::BTBossRingShockwave() {
    name_ = "BossRingShockwave";
}

BTBossRingShockwave::~BTBossRingShockwave() {
    OnCleanup();
}

Tako::BTNodeStatus BTBossRingShockwave::OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) {
    const float warningEnd = warningTime_;
    const float travelEnd = warningEnd + travelDuration_;
    const float fadeEnd = travelEnd + fadeTime_;

    if (elapsedTime_ < warningEnd) {
        // Warning Phase: プレイヤーへ追従しながら点滅
        const Vector3 dir = FacePlayerInstant(blackboard);
        if (dir.Length() > 0.0f) {
            lockedDir_ = dir;
            lockedYaw_ = atan2f(dir.x, dir.z);
        }
        originPos_ = boss->GetTranslate() + lockedDir_ * spawnOffsetForward_;

        const float sinValue = std::abs(std::sin(elapsedTime_ * blinkFrequency_ * std::numbers::pi_v<float>));
        UpdateRingVisual(boss, kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue);
    }
    else if (elapsedTime_ < travelEnd) {
        if (!hasLaunched_) {
            ringColliders_.Activate(ComputeCurrentScale() * (1.0f - Boss::kRingShockwaveWidthFraction * 0.5f));
            hasLaunched_ = true;
        }

        traveled_ += moveSpeed_ * deltaTime;
        UpdateRingVisual(boss, kRingBaseAlpha);

        const float scale = ComputeCurrentScale();
        const float widthFraction = Boss::kRingShockwaveWidthFraction;
        Vector3 center = originPos_ + lockedDir_ * traveled_;
        center.y = colliderY_;
        ringColliders_.UpdateArc(center, lockedYaw_, kSweepRad,
                                 scale * (1.0f - widthFraction * 0.5f), scale * widthFraction * colliderScale_,
                                 moveSpeed_ * deltaTime);
    }
    else {
        if (!hasEnded_) {
            ringColliders_.Deactivate();
            hasEnded_ = true;
        }

        if (elapsedTime_ < fadeEnd) {
            const float t = (fadeTime_ > 0.0f) ? (elapsedTime_ - travelEnd) / fadeTime_ : 1.0f;
            UpdateRingVisual(boss, kRingBaseAlpha * (1.0f - t));
        }
        else {
            boss->SetRingShockwaveVisible(false);
            if (cachedEmitterManager_) {
                cachedEmitterManager_->SetEmitterActive(emitterName_, false);
            }
        }
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossRingShockwave::OnInitialize(Tako::BTBlackboard* blackboard, Boss* boss) {
    (void)blackboard;

    hasLaunched_ = false;
    hasEnded_ = false;
    traveled_ = 0.0f;

    travelDuration_ = (moveSpeed_ > 0.0f) ? maxDistance_ / moveSpeed_ : 0.0f;
    totalDuration_ = warningTime_ + travelDuration_ + fadeTime_ + recoveryTime_;

    ringColliders_.Initialize(boss, segmentCount_);
    ringColliders_.SetDamage(damage_);

    lockedYaw_ = boss->GetRotate().y;
    lockedDir_ = Vector3(sinf(lockedYaw_), 0.0f, cosf(lockedYaw_));
    originPos_ = boss->GetTranslate() + lockedDir_ * spawnOffsetForward_;

    boss->SetRingShockwaveVisible(true);
    UpdateRingVisual(boss, kRingBaseAlpha);

    emitterName_ = boss->GetRingShockwaveEmitterName();
    if (cachedEmitterManager_) {
        cachedEmitterManager_->SetEmitterActive(emitterName_, true);
    }
}

float BTBossRingShockwave::ComputeCurrentScale() const {
    if (maxDistance_ <= 0.0f) return maxScale_;
    const float t = std::clamp(traveled_ / maxDistance_, 0.0f, 1.0f);
    return initialScale_ + (maxScale_ - initialScale_) * t;
}

void BTBossRingShockwave::UpdateRingVisual(Boss* boss, float alpha) {
    Tako::Actor* ringActor = boss ? boss->GetRingShockwaveActor() : nullptr;
    Object3d*    ring      = boss ? boss->GetRingShockwaveModel() : nullptr;
    if (!ringActor || !ring) return;

    const float scale = ComputeCurrentScale();
    const Vector3 pos = originPos_ + lockedDir_ * traveled_;
    // リングはボスの子でも、ワールドの中を進む
    ringActor->SetWorldTransform({ Vector3(scale, 1.0f, scale), Vector3(0.0f, lockedYaw_, 0.0f), Vector3(pos.x, spawnHeight_, pos.z) });
    // RGB は Object Settings の色のまま、アルファだけ演出で変える
    Vector4 color = ring->GetBaseMaterialColor();
    color.w = alpha;
    ring->SetMaterialColor(color);
    // WRAP サンプラーなので剰余を取らず加算のみでシームレスにループする
    ring->SetUvTransform(Transform{ Vector3(1.0f, 1.0f, 1.0f),
                                    Vector3(0.0f, 0.0f, 0.0f),
                                    Vector3(elapsedTime_ * uvScrollU_, elapsedTime_ * uvScrollV_, 0.0f) });
}

void BTBossRingShockwave::OnCleanup() {
    ringColliders_.Finalize();

    if (cachedBoss_) {
        cachedBoss_->SetRingShockwaveVisible(false);
        // 演出の上書きを外し、Object Settings の値に戻す
        if (Object3d* ring = cachedBoss_->GetRingShockwaveModel()) {
            ring->SetMaterialColor(std::nullopt);
            ring->SetUvTransform(std::nullopt);
        }
    }

    if (cachedEmitterManager_ && !emitterName_.empty()) {
        cachedEmitterManager_->SetEmitterActive(emitterName_, false);
    }

    hasLaunched_ = false;
    hasEnded_ = false;
    traveled_ = 0.0f;
}

