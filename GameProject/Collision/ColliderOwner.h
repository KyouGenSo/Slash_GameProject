#pragma once
#include "Actor.h"
#include "Collider.h"

#include "../Object/Boss/Boss.h"
#include "../Object/Player/Player.h"

/// <summary>
/// 層 BOSS のコライダーの持ち主。本体の当たり判定は ColliderComponent なので、GetOwner は Actor を返す
/// </summary>
/// <returns>Boss のスクリプトが付いていなければ nullptr</returns>
inline Boss* BossOf(const Tako::Collider* collider) {
    Tako::Actor* actor = static_cast<Tako::Actor*>(collider->GetOwner());
    return actor ? actor->GetScript<Boss>() : nullptr;
}

/// <summary>
/// 層 PLAYER のコライダーの持ち主。本体の当たり判定は ColliderComponent なので、GetOwner は Actor を返す
/// </summary>
/// <returns>Player のスクリプトが付いていなければ nullptr</returns>
inline Player* PlayerOf(const Tako::Collider* collider) {
    Tako::Actor* actor = static_cast<Tako::Actor*>(collider->GetOwner());
    return actor ? actor->GetScript<Player>() : nullptr;
}
