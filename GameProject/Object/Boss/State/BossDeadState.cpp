#include "BossDeadState.h"
#include "../Boss.h"

BossDeadState::BossDeadState()
	: BossState("Dead")
{
}

void BossDeadState::Enter(Boss* boss)
{
	(void)boss;
	// 死亡演出開始（現状は特別な演出なし）
}

void BossDeadState::Update(Boss* boss, float deltaTime)
{
	(void)boss;
	(void)deltaTime;
	// 死亡中は何もしない（シーン遷移はGameScene側で管理）
}

void BossDeadState::Exit(Boss* boss)
{
	(void)boss;
	// 復活時のクリーンアップ（デバッグ用）
}
