#pragma once
#include "PlayerState.h"

class AttackState : public PlayerState
{
public:
	AttackState() : PlayerState("Attack") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;

private:
	enum AttackPhase {
		SearchTarget,
		MoveToTarget,
		ExecuteAttack,
		Recovery
	};
	
	AttackPhase phase_ = SearchTarget;
	class Boss* targetEnemy_ = nullptr;
	float searchTimer_ = 0.0f;      // SearchTarget待機時間
	float maxSearchTime_ = 0.1f;    // 最大検索時間（0.1秒 = 6フレーム@60fps）
	float moveTimer_ = 0.0f;
	float maxMoveTime_ = 0.3f;
	
	float attackTimer_ = 0.0f;
	float attackDuration_ = 0.3f;
	int comboCount_ = 0;
	int maxCombo_ = 2;
	float comboWindow_ = 0.8f;
	bool canCombo_ = false;
	
	void SearchForTarget(Player* player);
	void ProcessMoveToTarget(Player* player, float deltaTime);
	void ProcessExecuteAttack(Player* player, float deltaTime);
	
public:
	// デバッグ用のゲッター
	AttackPhase GetPhase() const { return phase_; }
	Boss* GetTargetEnemy() const { return targetEnemy_; }
	float GetAttackTimer() const { return attackTimer_; }
	float GetSearchTimer() const { return searchTimer_; }
};