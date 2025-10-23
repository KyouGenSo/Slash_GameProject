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
	/// パリィ受付時間内か判定
	/// </summary>
	/// <returns>パリィウィンドウ内の場合true</returns>
	bool IsInParryWindow() const { return parryTimer_ <= parryWindow_; }

private:
	float parryTimer_ = 0.0f;          ///< パリィ経過時間
	float parryWindow_ = 0.2f;         ///< パーフェクトパリィの受付時間
	float parryDuration_ = 0.5f;       ///< パリィ全体の長さ
	bool perfectParryActive_ = false;  ///< パーフェクトパリィ成功フラグ
};