#include "BossMeleeAttackCollider.h"
#include "../Object/Player/Player.h"
#include "Settings/BossSettings.h"

#include "Actor.h"
#include "ColliderComponent.h"

using namespace Tako;

void BossMeleeAttackCollider::Awake() {
    collider_ = GetOwner()->GetComponent<ColliderComponent>();
    damage_   = Tako::GameSettings::Get<BossSettings>().melee.damage;
    SetActive(false);
}

void BossMeleeAttackCollider::OnCollisionEnter(Actor* other) {
    if (hasHitPlayer_) return;

    Player* player = other ? other->GetScript<Player>() : nullptr;
    if (!player) return;

    if (player->IsParrying()) {
        player->OnParrySuccess();
        hasHitPlayer_ = true;
        return;
    }

    player->OnHit(damage_);
    hasHitPlayer_ = true;
}

void BossMeleeAttackCollider::Reset() {
    hasHitPlayer_ = false;
}

void BossMeleeAttackCollider::SetActive(bool active)
{
    if (collider_) { collider_->SetActive(active); }
}

void BossMeleeAttackCollider::SetSize(const Tako::Vector3& size)
{
    if (collider_) { collider_->SetSize(size); }
}

void BossMeleeAttackCollider::SetOffset(const Tako::Vector3& offset)
{
    if (collider_) { collider_->SetOffset(offset); }
}
