#include "BTBossDash.h"
#include "Script/Object/Boss/Boss.h"
#include "Source/Object/Boss/Movement/BossAreaBounds.h"
#include "Script/Object/Player/Player.h"
#include "RandomEngine.h"
#include "EaseFunc.h"

#include <algorithm>
#include <cmath>

using namespace Tako;

BTBossDash::BTBossDash() {
    name_ = "BossDash";
}

Tako::BTNodeStatus BTBossDash::Execute(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    if (!boss) {
        status_ = Tako::BTNodeStatus::Failure;
        return Tako::BTNodeStatus::Failure;
    }

    float deltaTime = blackboard->GetDeltaTime();

    if (isFirstExecute_) {
        InitializeDash(boss);
        isFirstExecute_ = false;
        boss->SetDashing(true);
    }

    UpdateDashMovement(boss, deltaTime);

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= dashDuration_) {
        boss->SetTranslate(targetPosition_);

        boss->SetDashing(false);

        isFirstExecute_ = true;
        elapsedTime_ = 0.0f;
        status_ = Tako::BTNodeStatus::Success;
        return Tako::BTNodeStatus::Success;
    }

    status_ = Tako::BTNodeStatus::Running;
    return Tako::BTNodeStatus::Running;
}

void BTBossDash::Reset() {
    Tako::BTNode::Reset();
    elapsedTime_ = 0.0f;
    isFirstExecute_ = true;
    // boss 参照がないため SetDashing(false) は呼べない。状態を保持しない設計で安全。
}

void BTBossDash::InitializeDash(Boss* boss) {
    elapsedTime_ = 0.0f;

    startPosition_ = boss->GetTransform().translate;

    RandomEngine* rng = RandomEngine::GetInstance();

    // Y=0 で正規化済みの XZ 方向
    dashDirection_ = rng->GetRandomDirectionXZ();

    float dashDistance = rng->GetFloat(minDistance_, maxDistance_);

    targetPosition_ = startPosition_ + dashDirection_ * dashDistance;

    // Phase 2 では戦闘エリアが狭まるためエリア内に収める
    targetPosition_ = BossMovement::ClampToBounds(targetPosition_, BossMovement::CalcAreaBounds(boss));

    // Clamp 後の方向で再計算
    dashDirection_ = targetPosition_ - startPosition_;
    float actualDistance = dashDirection_.Length();
    if (actualDistance > kDirectionEpsilon) {
        dashDirection_ = dashDirection_.Normalize();
        dashDuration_ = actualDistance / dashSpeed_;
    }

    if (dashDirection_.Length() > kDirectionEpsilon) {
        float angle = atan2f(dashDirection_.x, dashDirection_.z);
        boss->SetRotate(Vector3(0.0f, angle, 0.0f));
    }
}

void BTBossDash::UpdateDashMovement(Boss* boss, float deltaTime) {
    deltaTime; // 未使用警告抑制
    if (elapsedTime_ < dashDuration_) {
        float t = elapsedTime_ / dashDuration_;

        t = Ease::SmoothStep(t);

        Vector3 newPosition = Vector3::Lerp(startPosition_, targetPosition_, t);
        boss->SetTranslate(newPosition);

        float vibration = sinf(elapsedTime_ * vibrationFreq_) * vibrationAmp_;
        Vector3 currentPos = boss->GetTransform().translate;
        currentPos.y += vibration;
        boss->SetTranslate(currentPos);
    }
}

#ifdef _DEBUG
#endif