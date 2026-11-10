#include "BossMeleeAttackCollider.h"
#include "../Object/Boss/Boss.h"
#include "ColliderOwner.h"
#include "CollisionTypeId.h"
#include "Settings/BossSettings.h"

using namespace Tako;

BossMeleeAttackCollider::BossMeleeAttackCollider(Boss* boss)
    : boss_(boss) {
    damage_ = Tako::GameSettings::Get<BossSettings>().melee.damage;

    SetTypeID(CollisionTypeId::BOSS_ATTACK);
    SetActive(false);
}

void BossMeleeAttackCollider::OnCollisionEnter(Collider* other) {
    if (!other || hasHitPlayer_) return;

    if (other->IsType(CollisionTypeId::PLAYER)) {
        Player* player = PlayerOf(other);
        if (!player) return;

        if (player->IsParrying()) {
            player->OnParrySuccess();
            hasHitPlayer_ = true;
            return;
        }

        player->OnHit(damage_);
        hasHitPlayer_ = true;
    }
}

void BossMeleeAttackCollider::OnCollisionStay(Collider* other) {
    // ダメージは1回のみ。継続ヒットなし
    (void)other;
}

void BossMeleeAttackCollider::Reset() {
    hasHitPlayer_ = false;
}
