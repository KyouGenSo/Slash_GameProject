#include "BTBossRapidFire.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include "Source/Common/GameConst.h"
#include <cmath>

using namespace Tako;

BTBossRapidFire::BTBossRapidFire() {
    name_ = "BossRapidFire";
}

Tako::BTNodeStatus BTBossRapidFire::OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) {
    if (elapsedTime_ < chargeTime_) {
        FacePlayerInstant(blackboard);
        bulletSignEffect_.Update(boss, deltaTime);
    }
    else if (firedCount_ < bulletCount_) {
        if (bulletSignEffect_.IsActive()) {
            bulletSignEffect_.End(boss);
        }

        FacePlayerInstant(blackboard);

        timeSinceLastFire_ += deltaTime;
        if (timeSinceLastFire_ >= fireInterval_) {
            FireBullet(blackboard);
            firedCount_++;
            timeSinceLastFire_ = 0.0f;

            if (firedCount_ >= bulletCount_) {
                EnterAttackRecovery(boss);
            }
        }
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        firedCount_ = 0;
        timeSinceLastFire_ = 0.0f;
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossRapidFire::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    firedCount_ = 0;
    // 即座に 1 発目を撃てるよう発射タイマを満タンに
    timeSinceLastFire_ = fireInterval_;
    totalDuration_ = chargeTime_ + (fireInterval_ * static_cast<float>(bulletCount_)) + recoveryTime_;
    bulletSignEffect_.Start(boss, chargeTime_);
}

void BTBossRapidFire::FireBullet(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    Vector3 firePosition = boss->GetTransform().translate;
    Vector3 direction = CalculateDirectionToPlayer(blackboard);
    Vector3 bulletVelocity = direction * bulletSpeed_;
    boss->RequestBulletSpawn(firePosition, bulletVelocity);
}

Vector3 BTBossRapidFire::CalculateDirectionToPlayer(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    Player* player = blackboard->GetPtr<Player>("player");
    if (!player) return Vector3(0.0f, 0.0f, 1.0f);

    Vector3 firePosition = boss->GetTransform().translate;
    Vector3 playerPos = player->GetTransform().translate;
    Vector3 toPlayer = playerPos - firePosition;

    float distance = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
    if (distance > GameConst::kDirectionEpsilon) {
        toPlayer = toPlayer.Normalize();
    } else {
        toPlayer = Vector3(0.0f, 0.0f, 1.0f);
    }
    return toPlayer;
}

