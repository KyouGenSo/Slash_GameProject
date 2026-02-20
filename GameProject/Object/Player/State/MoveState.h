#pragma once
#include "PlayerState.h"

/// <summary>
/// 移動状態クラス
/// プレイヤーが歩行・走行している時の状態
/// </summary>
class MoveState : public PlayerState
{
public:
	MoveState() : PlayerState("Walk") {}

	/// <summary>
	/// 移動状態開始時の処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void Enter(Player* player) override;

	/// <summary>
	/// 移動状態の更新処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	void Update(Player* player, float deltaTime) override;

	/// <summary>
	/// 移動状態終了時の処理
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

private:
	float moveTime_ = 0.0f;  ///< 移動継続時間
};