#include "BossMeleeAttackCollider.h"
#include "../Object/Boss/Boss.h"
#include "../Object/Player/Player.h"
#include "CollisionTypeIdDef.h"
#include "GlobalVariables.h"

using namespace Tako;

BossMeleeAttackCollider::BossMeleeAttackCollider(Boss* boss)
    : boss_(boss) {
    // GlobalVariables から値を取得
    GlobalVariables* gv = GlobalVariables::GetInstance();
    damage_ = gv->GetValueFloat("BossMeleeAttackCollider", "Damage");

    SetTypeID(static_cast<uint32_t>(CollisionTypeId::BOSS_ATTACK));
    SetActive(false);
}

void BossMeleeAttackCollider::OnCollisionEnter(Collider* other) {
    if (!other || hasHitPlayer_) return;

    uint32_t typeID = other->GetTypeID();

    // プレイヤーとの衝突判定
    if (typeID == static_cast<uint32_t>(CollisionTypeId::PLAYER)) {
        Player* player = static_cast<Player*>(other->GetOwner());

        // パリィ判定
        if (player->IsParrying()) {
            player->OnParrySuccess();
            hasHitPlayer_ = true;
            return;
        }

        // 通常ダメージ
        player->OnHit(damage_);
        hasHitPlayer_ = true;
    }
}

void BossMeleeAttackCollider::OnCollisionStay(Collider* other) {
    // 継続ヒットは行わない（1回のみダメージ）
    (void)other;
}

void BossMeleeAttackCollider::Reset() {
    hasHitPlayer_ = false;
}
