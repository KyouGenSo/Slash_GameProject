#include "BTBossBarrage.h"
#include "../../Boss.h"
#include "../../../Player/Player.h"
#include "../../../../Common/GameConst.h"
#include "RandomEngine.h"
#include "EaseFunc.h"

#include <cmath>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

BTBossBarrage::BTBossBarrage() {
    name_ = "BossBarrage";
}

Tako::BTNodeStatus BTBossBarrage::OnExecute(Tako::BTBlackboard* /*blackboard*/, Boss* boss, float deltaTime) {
    // フェーズ管理: Move → Charge → Firing → Recovery
    const float moveEnd = moveDuration_;
    const float chargeEnd = moveEnd + chargeTime_;
    const float firingEnd = chargeEnd + firingDuration_;

    if (elapsedTime_ < moveEnd) {
        UpdateMove(boss, deltaTime);
    }
    else if (elapsedTime_ < chargeEnd) {
        bulletSignEffect_.Update(boss, deltaTime);
    }
    else if (elapsedTime_ < firingEnd) {
        if (!hasEndedEffect_) {
            bulletSignEffect_.End(boss);
            hasEndedEffect_ = true;
        }

        timeSinceLastFire_ += deltaTime;
        if (timeSinceLastFire_ >= fireInterval_) {
            FireRandomBullet(boss);
            timeSinceLastFire_ = 0.0f;
        }
    }
    else {
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        timeSinceLastFire_ = 0.0f;
        hasEndedEffect_ = false;
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossBarrage::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    timeSinceLastFire_ = 0.0f;
    hasEndedEffect_ = false;

    totalDuration_ = moveDuration_ + chargeTime_ + firingDuration_ + recoveryTime_;

    startPosition_ = boss->GetTransform().translate;
    float targetX = (GameConst::StageXMin() + GameConst::StageXMax()) / 2.0f;
    float targetZ = (GameConst::StageZMin() + GameConst::StageZMax()) / 2.0f;
    targetPosition_ = Vector3(targetX, startPosition_.y, targetZ);

    bulletSignEffect_.Start(boss, chargeTime_);
}

void BTBossBarrage::UpdateMove(Boss* boss, float /*deltaTime*/) {
    if (elapsedTime_ < moveDuration_) {
        float t = elapsedTime_ / moveDuration_;
        t = Ease::SmoothStep(t);

        Vector3 newPosition = Vector3::Lerp(startPosition_, targetPosition_, t);
        boss->SetTranslate(newPosition);

        Vector3 direction = targetPosition_ - startPosition_;
        direction.y = 0.0f;
        if (direction.Length() > kDirectionEpsilon) {
            direction = direction.Normalize();
            float angle = atan2f(direction.x, direction.z);
            boss->SetRotate(Vector3(0.0f, angle, 0.0f));
        }
    } else {
        boss->SetTranslate(targetPosition_);
    }
}

void BTBossBarrage::FireRandomBullet(Boss* boss) {
    Vector3 firePosition = boss->GetTransform().translate;

    RandomEngine* rng = RandomEngine::GetInstance();
    Vector3 direction = rng->GetRandomDirectionXZ();

    bool isPenetrating = rng->GetBool(penetratingRatio_);

    float speed;
    if (isPenetrating) {
        speed = rng->GetFloat(penetratingBulletSpeedMin_, penetratingBulletSpeedMax_);
        boss->RequestPenetratingBulletSpawn(firePosition, direction * speed);
    } else {
        speed = rng->GetFloat(normalBulletSpeedMin_, normalBulletSpeedMax_);
        boss->RequestBulletSpawn(firePosition, direction * speed);
    }
}

#ifdef _DEBUG
bool BTBossBarrage::DrawCustomImGui() {
    ImGui::Text("Fire Rate: %.1f shots/sec", 1.0f / fireInterval_);
    return false;
}
#endif
