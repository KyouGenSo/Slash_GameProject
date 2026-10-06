#include "BossMeleeAttackCollider.h"
#include "../Object/Boss/Boss.h"
#include "../Object/Player/Player.h"
#include "CollisionTypeId.h"
#include "GlobalVariables.h"

using namespace Tako;

BossMeleeAttackCollider::BossMeleeAttackCollider(Boss* boss)
    : boss_(boss) {
    GlobalVariables* gv = GlobalVariables::GetInstance();
    damage_ = gv->GetValueFloat("BossMeleeAttackCollider", "Damage");

    SetTypeID(CollisionTypeId::BOSS_ATTACK);
    SetActive(false);
}

void BossMeleeAttackCollider::OnCollisionEnter(Collider* other) {
    if (!other || hasHitPlayer_) return;

    if (other->IsType(CollisionTypeId::PLAYER)) {
        Player* player = static_cast<Player*>(other->GetOwner());

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
