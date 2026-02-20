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

	// フラッシュタイマー初期化＋初回フラッシュ
	flashTimer_ = 0.0f;
	boss->StartStunFlash(stunFlashColor_, flashDuration_);
}

void BossPhaseTransitionStunState::Update(Boss* boss, float deltaTime)
{
	// パーティクル位置をボスに追従させる
	boss->SetCanAttackSignEmitterPosition(boss->GetTransform().translate);

	// 色点滅処理（スタン中継続）
	flashTimer_ += deltaTime;
	if (flashTimer_ >= flashInterval_) {
		flashTimer_ = 0.0f;
		boss->StartStunFlash(stunFlashColor_, flashDuration_);
	}

	// 被弾によるフェーズ2移行は Boss::OnMeleeAttackHit()から
	// CompletePhaseTransition()を呼び出して処理する
}

void BossPhaseTransitionStunState::Exit(Boss* boss)
{
	// パーティクルを無効化
	boss->SetCanAttackSignEmitterActive(false);

	// フラッシュタイマーリセット
	flashTimer_ = 0.0f;
}
