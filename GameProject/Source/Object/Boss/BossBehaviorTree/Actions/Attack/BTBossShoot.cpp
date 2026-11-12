#include "BTBossShoot.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include <cmath>

using namespace Tako;

BTBossShoot::BTBossShoot() {
    name_ = "BossShoot";
}

Tako::BTNodeStatus BTBossShoot::OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) {
    if (elapsedTime_ < chargeTime_) {
        FacePlayerInstant(blackboard);
        bulletSignEffect_.Update(boss, deltaTime);
    }

    if (elapsedTime_ >= chargeTime_ && !hasFired_) {
        bulletSignEffect_.End(boss);
        FireBullets(blackboard);
        hasFired_ = true;
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossShoot::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    totalDuration_ = chargeTime_ + recoveryTime_;
    hasFired_ = false;
    bulletSignEffect_.Start(boss, chargeTime_);
}

void BTBossShoot::FireBullets(Tako::BTBlackboard* blackboard) {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    Player* player = blackboard->GetPtr<Player>("player");
    if (!player) return;

    Vector3 firePosition = boss->GetTransform().translate;
    Vector3 playerPos = player->GetTransform().translate;
    Vector3 toPlayer = playerPos - firePosition;

    float distance = sqrtf(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
    if (distance > kDirectionEpsilon) {
        toPlayer = toPlayer.Normalize();
    }

    for (int i = 0; i < bulletCount_; i++) {
        float angleOffset = 0.0f;
        if (bulletCount_ > 1) {
            float t = static_cast<float>(i) / static_cast<float>(bulletCount_ - 1);
            angleOffset = spreadAngle_ * (2.0f * t - 1.0f);
        }
        Vector3 bulletDirection = CalculateBulletDirection(toPlayer, angleOffset);
        Vector3 bulletVelocity = bulletDirection * bulletSpeed_;
        boss->RequestBulletSpawn(firePosition, bulletVelocity);
    }
}

Vector3 BTBossShoot::CalculateBulletDirection(const Vector3& baseDirection, float angleOffset) {
    if (std::abs(angleOffset) < kAngleEpsilon) return baseDirection;

    float cos_angle = cosf(angleOffset);
    float sin_angle = sinf(angleOffset);

    Vector3 rotatedDirection;
    rotatedDirection.x = baseDirection.x * cos_angle - baseDirection.z * sin_angle;
    rotatedDirection.y = baseDirection.y;
    rotatedDirection.z = baseDirection.x * sin_angle + baseDirection.z * cos_angle;
    return rotatedDirection.Normalize();
}

