#include "BossAreaAttackCollider.h"
#include "../Object/Boss/Boss.h"
#include "CollisionTypeId.h"
#include "PlayerHitResolver.h"

using namespace Tako;

BossAreaAttackCollider::BossAreaAttackCollider(Boss* boss)
    : boss_(boss) {
    SetTypeID(CollisionTypeId::BOSS_ATTACK);
    SetActive(false);
}

void BossAreaAttackCollider::OnCollisionEnter(Collider* other) {
    ResolvePlayerHit(other, damage_);
}
