#include "BossAttackHitbox.h"
#include "../Object/Boss/Boss.h"
#include "../Object/Player/Player.h"

#include "Actor.h"
#include "BaseScene.h"
#include "ColliderComponent.h"
#include "Logger.h"

using namespace Tako;

void BossAttackHitbox::OnCollisionEnter(Actor* other) {
    Player* player = other ? other->GetScript<Player>() : nullptr;
    if (!player) return;

    if (sharedHitGuard_) {
        if (*sharedHitGuard_) return;
        *sharedHitGuard_ = true;
    }
    if (player->IsParrying()) {
        player->OnParrySuccess();
        return;
    }
    player->OnHit(damage_);
}

void BossAttackHitbox::SetActive(bool active) {
    if (ColliderComponent* collider = GetOwner()->GetComponent<ColliderComponent>()) { collider->SetActive(active); }
}

void BossAttackHitbox::SetTransform(const Tako::Transform& transform) {
    GetOwner()->SetTransform(transform);
}

void BossAttackHitbox::SetRadius(float radius) {
    if (ColliderComponent* collider = GetOwner()->GetComponent<ColliderComponent>()) { collider->SetRadius(radius); }
}

void BossAttackHitbox::SetSize(const Tako::Vector3& size) {
    if (ColliderComponent* collider = GetOwner()->GetComponent<ColliderComponent>()) { collider->SetSize(size); }
}

void BossAttackHitboxRef::Spawn(Boss* boss, const std::string& prefab, const Tako::Transform& transform) {
    Destroy();
    scene_ = boss->GetOwner()->GetScene();
    Actor*            actor  = scene_->Spawn(prefab);
    BossAttackHitbox* hitbox = actor ? actor->GetScript<BossAttackHitbox>() : nullptr;
    if (!hitbox) {
        Logger::Log("BossAttackHitbox: the prefab \"%s\" is missing or has no BossAttackHitbox script. The attack will not hit\n", prefab.c_str());
        if (actor) { actor->Destroy(); }
        return;
    }
    id_ = actor->GetId();
    hitbox->SetTransform(transform);
    hitbox->SetActive(false);
}

void BossAttackHitboxRef::Destroy() {
    if (BossAttackHitbox* hitbox = Get()) {
        // 消えるのは次のフレームなので、このフレームの当たり判定で、消えたノードの値（共有ガード）を触らせない
        hitbox->SetActive(false);
        hitbox->SetSharedHitGuard(nullptr);
        hitbox->GetOwner()->Destroy();
    }
    id_ = 0;
}

BossAttackHitbox* BossAttackHitboxRef::Get() const {
    Actor* actor = scene_ && id_ != 0 ? scene_->FindActor(id_) : nullptr;
    return actor ? actor->GetScript<BossAttackHitbox>() : nullptr;
}
