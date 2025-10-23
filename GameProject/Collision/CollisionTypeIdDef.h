#pragma once
#include <cstdint>

/// <summary>
/// 衝突判定タイプID定義
/// オブジェクトのカテゴリを識別して衝突処理を制御
/// </summary>
enum class CollisionTypeId : uint32_t {
	kDefault,           /// デフォルトタイプ
	kPlayer,            /// プレイヤー本体
	kPlayerMeleeAttack, /// プレイヤーの近接攻撃
	kPlayerBullet,      /// プレイヤーの弾丸
	kEnemy,             /// 敵キャラクター
	kEnemyAttack,       /// 敵の攻撃
	kEnvironment,       /// 環境オブジェクト
};