#include "MeleeAttackCollider.h"
#include "Player.h"
#include "../Boss/Boss.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include "GlobalVariables.h"

MeleeAttackCollider::MeleeAttackCollider(Player* player)
    : player_(player) {
    // GlobalVariablesから値を取得
    GlobalVariables* gv = GlobalVariables::GetInstance();
    attackDamage_ = gv->GetValueFloat("MeleeAttack", "AttackDamage");

    SetTypeID(static_cast<uint32_t>(CollisionTypeId::PLAYER_MELEE_ATTACK));
    SetActive(false);
}

void MeleeAttackCollider::OnCollisionEnter(Collider* other) {
    if (!other) return;

    uint32_t typeID = other->GetTypeID();

    if (typeID == static_cast<uint32_t>(CollisionTypeId::BOSS)) {
        Boss* enemy = static_cast<Boss*>(other->GetOwner());
        if (enemy) {

            enemy->OnHit(attackDamage_);

            if (!detectedEnemy_) {
                detectedEnemy_ = enemy;
            }
        }
    }
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
            if (canDamage)
            {
                enemy->OnHit(attackDamage_);
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