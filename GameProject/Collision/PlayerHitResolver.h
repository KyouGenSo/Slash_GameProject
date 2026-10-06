#pragma once
#include <cstdint>

#include "Collider.h"
#include "CollisionTypeId.h"
#include "../Object/Player/Player.h"

/// <summary>
/// 相手が PLAYER のとき、パリィ中なら OnParrySuccess、否なら OnHit(damage)
/// </summary>
/// <param name="other">衝突相手のコライダー。nullptr や PLAYER 以外なら何もしない</param>
/// <param name="damage">パリィしていないプレイヤーに与えるダメージ量</param>
inline void ResolvePlayerHit(Tako::Collider* other, float damage) {
    if (!other) return;
    if (!other->IsType(CollisionTypeId::PLAYER)) return;

    Player* player = static_cast<Player*>(other->GetOwner());
    if (player->IsParrying()) {
        player->OnParrySuccess();
        return;
    }
    player->OnHit(damage);
}
