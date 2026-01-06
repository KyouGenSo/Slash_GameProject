#pragma once
#include "PlayerState.h"

/// <summary>
/// パリィ状態クラス
/// 敵の攻撃を防御・反撃するタイミング管理
/// </summary>
class ParryState : public PlayerState
{
public:
	ParryState() : PlayerState("Parry") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;
	
	/// <summary>
	/// パリィ成功時の処理
	/// </summary>
	/// <param name="player">プレイヤーインスタンス</param>
	void OnParrySuccess(Player* player);

	/// <summary>
	/// ImGuiデバッグ情報の描画
	/// </summary>
	void DrawImGui(Player* player) override;

	// Getter
	float GetParryTimer() const { return parryTimer_; }
	float GetParryDuration() const { return parryDuration_; }

	// Setter
	void SetParryDuration(float duration) { parryDuration_ = duration; }

private:
	float parryTimer_          = 0.0f;          ///< パリィ経過時間
	float parryDuration_       = 0.5f;          ///< パリィ全体の長さ
};