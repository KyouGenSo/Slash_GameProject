#include "Bullet.h"
#include "Actor.h"
#include "BaseScene.h"
#include "ForceFieldManager.h"
#include "ParticleSystemComponent.h"

#include "../Common/ForceFieldAffectMask.h"
#include "../Common/GameConst.h"
#include "../Object/Boss/Boss.h"
#include "../Object/Player/Player.h"

namespace {

    constexpr const char* kExplodeComponent = "Explode";
    constexpr float       kExplodeLifetime  = 0.5f;
    constexpr float       kBossHitShake     = 0.5f;

}

void Bullet::Spawn(Tako::BaseScene& scene, std::string_view prefab, const Tako::Vector3& position, const Tako::Vector3& velocity) {
    Tako::Actor* actor = scene.Spawn(prefab);
    if (!actor) {
        return;
    }
    actor->GetTransform().translate = position;
    if (Bullet* bullet = actor->GetScript<Bullet>()) {
        bullet->velocity_ = velocity;
    }
}

void Bullet::Tick(float deltaTime) {
    if (isFinished_) {
        return;
    }
    Tako::Vector3& position = GetOwner()->GetTransform().translate;

    // GPU パーティクルと同じ力場を、CPU で動かす弾にも作用させる
    if (affectedByForceField_) {
        velocity_ += GetOwner()->GetScene()->GetForceFieldManager().EvaluateForceAt(position, GameForceField::AffectBullets) * deltaTime;
    }
    position += velocity_ * deltaTime;
    elapsedTime_ += deltaTime;

    const bool isOutside = position.x < GameConst::StageXMin() || position.x > GameConst::StageXMax()
                        || position.z < GameConst::StageZMin() || position.z > GameConst::StageZMax()
                        || position.y < minY_ || position.y > maxY_;
    const bool isTooSlow = minSpeed_ > 0.0f && velocity_.LengthSquared() < minSpeed_ * minSpeed_;
    if (elapsedTime_ >= lifetime_ || isOutside || isTooSlow) {
        Finish();
    }
}

void Bullet::OnCollisionEnter(Tako::Actor* other) {
    // Actor でないコライダー（攻撃の当たり判定など）は相手にしない
    if (isFinished_ || !other) {
        return;
    }
    if (target_ == Target::Player) {
        if (Player* player = other->GetScript<Player>()) {
            if (player->IsParrying()) {
                player->OnParrySuccess();
            } else {
                player->OnHit(damage_);
            }
            Finish();
            return;
        }
    } else if (Boss* boss = other->GetScript<Boss>()) {
        if (!boss->IsInPhaseTransitionStun()) {
            boss->OnHit(damage_, kBossHitShake);
        }
        Finish();
        return;
    }
    if (cancelsOnEnemyBullet_) {
        const Bullet* enemy = other->GetScript<Bullet>();
        if (enemy && enemy->target_ != target_) {
            Finish();
        }
    }
}

void Bullet::Finish() {
    isFinished_ = true;
    Tako::Component* explode = GetOwner()->FindComponent(kExplodeComponent);
    if (explode && explode->IsA<Tako::ParticleSystemComponent>()) {
        static_cast<Tako::ParticleSystemComponent*>(explode)->EmitTemporary(kExplodeLifetime);
    }
    GetOwner()->Destroy();
}
