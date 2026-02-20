#pragma once
#include "PlayerState.h"
#include "Vector3.h"

/// <summary>
/// 射撃状態クラス
/// 遠距離攻撃の照準と発射を制御
/// </summary>
class ShootState : public PlayerState
{
public:
	ShootState() : PlayerState("Shoot") {}

	/// <summary>
	/// 射撃状態開始時の処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Enter(Player* player) override;

	/// <summary>
	/// 射撃状態の更新処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	void Update(Player* player, float deltaTime) override;

	/// <summary>
	/// 射撃状態終了時の処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Exit(Player* player) override;

	/// <summary>
	/// 入力処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void HandleInput(Player* player) override;

	/// <summary>
	/// ImGui デバッグ情報の描画
	/// </summary>
	void DrawImGui(Player* player) override;

	// DrawImGui 用のゲッター
	float GetFireRate() const { return fireRate_; }
	float GetFireRateTimer() const { return fireRateTimer_; }
	const Tako::Vector3& GetAimDirection() const { return aimDirection_; }

	// DrawImGui 用のセッター（デバッグ調整用）
	void SetFireRate(float rate) { fireRate_ = rate; }

private:
	float fireRate_ = 0.2f;              ///< 発射レート（秒間隔）
	float fireRateTimer_ = 0.0f;         ///< 発射間隔タイマー
	float moveSpeedMultiplier_ = 0.5f;   ///< 射撃中の移動速度倍率
    Tako::Vector3 aimDirection_;                ///< 照準方向ベクトル

	/// <summary>
	/// 照準方向を計算
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void CalculateAimDirection(Player* player);

	/// <summary>
	/// 弾を発射
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Fire(Player* player);
};