#include "BossNormalState.h"
#include "../Boss.h"
#include "../BossBehaviorTree/BossBehaviorTree.h"

BossNormalState::BossNormalState()
	: BossState("Normal")
{
}

void BossNormalState::Enter(Boss* boss)
{
	// BT再開（ResetはしないーRunning中のアクションを継続可能にする）
}

void BossNormalState::Update(Boss* boss, float deltaTime)
{
	BossBehaviorTree* bt = boss->GetBehaviorTree();
	if (bt) {
		bt->Update(deltaTime);
	}
}

void BossNormalState::Exit(Boss* boss)
{
	// 行動状態をクリーン（攻撃中断時のエフェクト等を確実に消す）
	boss->ResetActionState();

	// BTをリセット（Running中のアクションを中断）
	BossBehaviorTree* bt = boss->GetBehaviorTree();
	if (bt) {
		bt->Reset();
	}
}
