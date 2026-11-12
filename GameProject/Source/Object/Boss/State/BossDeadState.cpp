#include "BossDeadState.h"
#include "Script/Object/Boss/Boss.h"

BossDeadState::BossDeadState()
	: BossState("Dead")
{
}

void BossDeadState::Enter(Boss* boss)
{
	(void)boss;
	// 死亡演出なし
}

void BossDeadState::Update(Boss* boss, float deltaTime)
{
	(void)boss;
	(void)deltaTime;
	// シーン遷移は GameDirector が行う
}

void BossDeadState::Exit(Boss* boss)
{
	(void)boss;
	// 復活（デバッグ用）時のクリーンアップ
}
