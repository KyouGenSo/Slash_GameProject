#pragma once
#include "BossState.h"

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
};
