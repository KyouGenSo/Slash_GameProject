#include "MeleeAttackCollider.h"
#include "Script/Object/Boss/Boss.h"

#include "Actor.h"
#include "ColliderComponent.h"

using namespace Tako;

void MeleeAttackCollider::Awake() {
    collider_ = GetOwner()->GetComponent<ColliderComponent>();
    SetActive(false);
}

void MeleeAttackCollider::OnCollisionStay(Actor* other) {
#ifdef _DEBUG
    collisionCount_++;
#endif

    Boss* enemy = other ? other->GetScript<Boss>() : nullptr;
    if (enemy && !detectedEnemy_) {
        detectedEnemy_ = enemy;

        if (canDamage_) {
            // ノックバック方向（プレイヤー→ボス）。水平面のみ
            Tako::Vector3 knockbackDir = enemy->GetTransform().translate - GetOwner()->GetWorldPosition();
            knockbackDir.y = 0.0f;
            if (knockbackDir.Length() > 0.01f) {
                knockbackDir = knockbackDir.Normalize();
            }

            enemy->OnMeleeAttackHit(attackDamage_, knockbackDir, knockbackEnabled_);
            canDamage_ = false;
        }
    }
}

void MeleeAttackCollider::Reset() {
    detectedEnemy_ = nullptr;
#ifdef _DEBUG
    collisionCount_ = 0;
#endif
}

void MeleeAttackCollider::Damage()
{
    canDamage_ = true;
}

void MeleeAttackCollider::SetActive(bool active)
{
    if (collider_) { collider_->SetActive(active); }
}

void MeleeAttackCollider::SetSize(const Tako::Vector3& size)
{
    if (collider_) { collider_->SetSize(size); }
}

void MeleeAttackCollider::SetOffset(const Tako::Vector3& offset)
{
    if (collider_) { collider_->SetOffset(offset); }
}

bool MeleeAttackCollider::IsActive() const
{
    return collider_ && collider_->IsActive();
}
