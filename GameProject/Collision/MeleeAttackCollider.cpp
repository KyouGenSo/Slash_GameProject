#include "MeleeAttackCollider.h"
#include "../Object/Player/Player.h"
#include "../Object/Boss/Boss.h"
#include "CollisionTypeIdDef.h"
#include "GlobalVariables.h"
#include "../CameraSystem/CameraManager.h"

using namespace Tako;

MeleeAttackCollider::MeleeAttackCollider(Player* player)
    : player_(player) {
    // GlobalVariablesから値を取得
    GlobalVariables* gv = GlobalVariables::GetInstance();
    attackDamage_ = gv->GetValueFloat("MeleeAttack", "AttackDamage");

    SetTypeID(static_cast<uint32_t>(CollisionTypeId::PLAYER_ATTACK));
    SetActive(false);
}

void MeleeAttackCollider::OnCollisionEnter(Collider* other) {
    if (!other) return;

    uint32_t typeID = other->GetTypeID();

    if (typeID == static_cast<uint32_t>(CollisionTypeId::BOSS)) {
        Boss* enemy = static_cast<Boss*>(other->GetOwner());
        if (enemy) {
            // ダッシュ中は無視（何も起こらない）
            if (enemy->IsDashing()) {
                return;
            }

            // ノックバック/離脱方向を計算（プレイヤー → ボス）
            Tako::Vector3 knockbackDir = enemy->GetTransform().translate - player_->GetTransform().translate;
            knockbackDir.y = 0.0f;
            if (knockbackDir.Length() > 0.01f) {
                knockbackDir = knockbackDir.Normalize();
            }

            // スタン中: ダメージのみ（スタン延長なし）
            if (enemy->IsStunned()) {
                enemy->OnHit(attackDamage_, 1.0f);
                CameraManager::GetInstance()->StartShake(0.3f);
            }
            // 硬直中: ダメージ＋スタン
            else if (enemy->IsInRecovery()) {
                enemy->OnHit(attackDamage_, 1.0f);
                CameraManager::GetInstance()->StartShake(0.3f);
                enemy->TriggerStun(knockbackDir);
            }
            // それ以外: 離脱トリガー（ダメージなし）
            else {
                enemy->TriggerRetreat(knockbackDir);
                detectedEnemy_ = enemy;  // 同一衝突セッション中の再トリガー防止
                return;  // 離脱トリガー後は他の処理をスキップ
            }

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
            // ダッシュ中は無視（何も起こらない）
            if (enemy->IsDashing()) {
                return;
            }

            detectedEnemy_ = enemy;
            if (canDamage)
            {
                // ノックバック/離脱方向を計算（プレイヤー → ボス）
                Tako::Vector3 knockbackDir = enemy->GetTransform().translate - player_->GetTransform().translate;
                knockbackDir.y = 0.0f;
                if (knockbackDir.Length() > 0.01f) {
                    knockbackDir = knockbackDir.Normalize();
                }

                // スタン中: ダメージのみ（スタン延長なし）
                if (enemy->IsStunned()) {
                    enemy->OnHit(attackDamage_, 1.0f);
                    CameraManager::GetInstance()->StartShake(0.3f);
                }
                // 硬直中: ダメージ＋スタン
                else if (enemy->IsInRecovery()) {
                    enemy->OnHit(attackDamage_, 1.0f);
                    CameraManager::GetInstance()->StartShake(0.3f);
                    enemy->TriggerStun(knockbackDir);
                }
                // それ以外: 離脱トリガー（ダメージなし）
                else {
                    enemy->TriggerRetreat(knockbackDir);
                }

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