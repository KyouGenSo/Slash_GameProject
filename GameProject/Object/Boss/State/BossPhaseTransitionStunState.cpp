#include "BossPhaseTransitionStunState.h"
#include "../Boss.h"

BossPhaseTransitionStunState::BossPhaseTransitionStunState()
	: BossState("PhaseTransitionStun")
{
}

void BossPhaseTransitionStunState::Enter(Boss* boss)
{
	// 攻撃可能サインパーティクルを有効化
	boss->SetCanAttackSignEmitterActive(true);
	boss->SetCanAttackSignEmitterPosition(boss->GetTransform().translate);
}

void BossPhaseTransitionStunState::Update(Boss* boss, float deltaTime)
{
	(void)deltaTime;

	// パーティクル位置をボスに追従させる
	boss->SetCanAttackSignEmitterPosition(boss->GetTransform().translate);

	// 被弾によるフェーズ2移行はBoss::OnMeleeAttackHit()から
	// CompletePhaseTransition()を呼び出して処理する
	// この状態はタイムアウトしない（プレイヤーの攻撃待ち）
}

void BossPhaseTransitionStunState::Exit(Boss* boss)
{
	// パーティクルを無効化
	boss->SetCanAttackSignEmitterActive(false);
}
