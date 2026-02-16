#pragma once
#include "BossState.h"

/// <summary>
/// ボスの通常状態
/// BehaviorTreeがAI意思決定を行い、各行動を実行する
/// </summary>
class BossNormalState : public BossState {
public:
	BossNormalState();
	~BossNormalState() override = default;

	void Enter(Boss* boss) override;
	void Update(Boss* boss, float deltaTime) override;
	void Exit(Boss* boss) override;
};
