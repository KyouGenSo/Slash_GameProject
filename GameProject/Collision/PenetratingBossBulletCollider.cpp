#include "PenetratingBossBulletCollider.h"
#include "../Object/Player/Player.h"
#include "../Object/Projectile/PenetratingBossBullet.h"
#include "CollisionTypeIdDef.h"
#include "CollisionManager.h"

PenetratingBossBulletCollider::PenetratingBossBulletCollider(PenetratingBossBullet* owner)
    : owner_(owner) {
}

void PenetratingBossBulletCollider::OnCollisionEnter(Tako::Collider* other) {
    if (!owner_ || !owner_->IsActive()) {
        return;
    }

    // プレイヤーとの衝突判定
    if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeId::PLAYER)) {
        // 多重ヒット防止チェック
        void* targetPtr = other->GetOwner();
        if (hitTargets_.find(targetPtr) != hitTargets_.end()) {
            return;  // 既にヒット済み
        }

        // プレイヤーの取得
        Player* player = static_cast<Player*>(other->GetOwner());
        if (player) {
            hitPlayer_ = player;
            hitTargets_.insert(targetPtr);

            // パリィ判定
            if (player->IsParrying()) {
                player->OnParrySuccess();
                owner_->SetActive(false);
                return;
            }

            // プレイヤーにダメージを与える
            player->OnHit(owner_->GetDamage());
            hasDealtDamage_ = true;

            // 弾を非アクティブ化
            owner_->SetActive(false);
        }
    }
    // プレイヤーの弾との衝突時は何もしない（貫通）
    // PLAYER_ATTACKとの衝突判定は行わない
}

void PenetratingBossBulletCollider::OnCollisionStay(Tako::Collider* other) {
    // 貫通弾は継続的な処理は行わない
}

void PenetratingBossBulletCollider::OnCollisionExit(Tako::Collider* other) {
    // 衝突終了処理
    if (other->GetOwner() == hitPlayer_) {
        hitPlayer_ = nullptr;
    }
}

void PenetratingBossBulletCollider::Reset() {
    hitTargets_.clear();
    hitPlayer_ = nullptr;
    hasDealtDamage_ = false;
}
