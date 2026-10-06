// 自動生成（Engine Settings の Save で上書きされるので直接編集しない）
#pragma once
#include <cstdint>

/// <summary>
/// 衝突層（Engine Settings > Physics > Layers。値はコライダーの型 ID）
/// </summary>
enum class CollisionTypeId : uint32_t {
    DEFAULT = 0,
    PLAYER = 1,
    PLAYER_ATTACK = 2,
    PLAYER_PROJECTILE = 3,
    BOSS = 4,
    BOSS_ATTACK = 5,
    BOSS_PROJECTILE = 6,
    ENVIRONMENT = 7,
};
