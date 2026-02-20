#include "BossNormalState.h"
#include "../Boss.h"
#include "../BossBehaviorTree/BossBehaviorTree.h"

BossNormalState::BossNormalState()
    : BossState("Normal")
{
}

void BossNormalState::Enter(Boss* boss)
{
    // BT を初期化
    BossBehaviorTree* bt = boss->GetBehaviorTree();
    if (bt) {
        bt->Reset();
    }
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

    // BT をリセット（Running 中のアクションを中断）
    BossBehaviorTree* bt = boss->GetBehaviorTree();
    if (bt) {
        bt->Reset();
    }
}
