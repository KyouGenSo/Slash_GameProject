#pragma once
#include "Vector3.h"

/// <summary>
/// 弾生成リクエスト構造体
/// Player/Bossが弾の生成をGameSceneに要求する際に使用
/// </summary>
struct BulletSpawnRequest {
    Tako::Vector3 position;  ///< 発射位置
    Tako::Vector3 velocity;  ///< 弾の速度ベクトル
};
