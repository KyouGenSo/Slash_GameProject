#pragma once
#include "BossState.h"

/// <summary>
/// ボスの死亡状態
/// HP=0で永続遷移し、死亡演出を管理
/// </summary>
class BossDeadState : public BossState {
public:
	BossDeadState();
	~BossDeadState() override = default;

	void Enter(Boss* boss) override;
	void Update(Boss* boss, float deltaTime) override;
	void Exit(Boss* boss) override;
};
