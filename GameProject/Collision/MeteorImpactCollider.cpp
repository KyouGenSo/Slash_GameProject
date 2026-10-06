#include "MeteorImpactCollider.h"
#include "../Object/Boss/Boss.h"
#include "CollisionTypeId.h"
#include "PlayerHitResolver.h"

using namespace Tako;

MeteorImpactCollider::MeteorImpactCollider(Boss* boss)
    : boss_(boss) {
    SetTypeID(CollisionTypeId::BOSS_ATTACK);
    SetActive(false);
}

void MeteorImpactCollider::OnCollisionEnter(Collider* other) {
    const bool isPlayer = other && other->IsType(CollisionTypeId::PLAYER);
    if (sharedHitGuard_ && isPlayer) {
        if (*sharedHitGuard_) return;
        *sharedHitGuard_ = true;
    }
    ResolvePlayerHit(other, damage_);
}
