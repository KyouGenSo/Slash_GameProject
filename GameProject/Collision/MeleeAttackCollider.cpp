#include "MeleeAttackCollider.h"
#include "../Object/Player/Player.h"
#include "../Object/Boss/Boss.h"
#include "CollisionTypeIdDef.h"
#include "GlobalVariables.h"

using namespace Tako;

MeleeAttackCollider::MeleeAttackCollider(Player* player)
    : player_(player) {
    // GlobalVariablesから値を取得
    GlobalVariables* gv = GlobalVariables::GetInstance();
    attackDamage_ = gv->GetValueFloat("MeleeAttack", "AttackDamage");

    SetTypeID(static_cast<uint32_t>(CollisionTypeId::PLAYER_ATTACK));
    SetActive(false);
}

void MeleeAttackCollider::OnCollisionStay(Collider* other) {
    if (!other) return;

#ifdef _DEBUG
    collisionCount_++;
#endif

    uint32_t typeID = other->GetTypeID();

    if (typeID == static_cast<uint32_t>(CollisionTypeId::BOSS)) {
        Boss* enemy = static_cast<Boss*>(other->GetOwner());
        if (enemy && !detectedEnemy_) {
            detectedEnemy_ = enemy;
            if (canDamage) {
                // ノックバック方向を計算（プレイヤー → ボス）
                Tako::Vector3 knockbackDir = enemy->GetTransform().translate - player_->GetTransform().translate;
                knockbackDir.y = 0.0f;
                if (knockbackDir.Length() > 0.01f) {
                    knockbackDir = knockbackDir.Normalize();
                }
                // ボス側で状態に応じた判定を行う
                enemy->OnMeleeAttackHit(attackDamage_, knockbackDir, knockbackEnabled_);
                canDamage = false;
            }
        }
    }
}

void MeleeAttackCollider::Reset() {
    hitEnemies_.clear();
    detectedEnemy_ = nullptr;
#ifdef _DEBUG
    collisionCount_ = 0;
#endif
}

void MeleeAttackCollider::Damage()
{
    canDamage = true;
}

bool MeleeAttackCollider::HasHitEnemy(uint32_t enemyId) const {
    return hitEnemies_.find(enemyId) != hitEnemies_.end();
}