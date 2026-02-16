#pragma once
#include "BossState.h"
#include "Vector4.h"

/// <summary>
/// ボスのフェーズ移行スタン状態
/// HP閾値到達で強制遷移し、プレイヤーの近接攻撃を待つ
/// 被弾でフェーズ2へ移行しNormalに復帰
/// </summary>
class BossPhaseTransitionStunState : public BossState {
public:
	BossPhaseTransitionStunState();
	~BossPhaseTransitionStunState() override = default;

	void Enter(Boss* boss) override;
	void Update(Boss* boss, float deltaTime) override;
	void Exit(Boss* boss) override;

private:
	// フラッシュパラメータ（BossStunnedStateと同一値）
	float flashInterval_ = 0.05f;
	float flashDuration_ = 0.03f;
	Tako::Vector4 stunFlashColor_ = { 1.0f, 1.0f, 0.0f, 1.0f };

	// 状態管理
	float flashTimer_ = 0.0f;
};
