#include "IdleState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "Input.h"
#include "Input/InputHandler.h"

void IdleState::Enter(Player* player)
{
	// モデルのアイドルアニメーションを再生
  // TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Idle");
}

void IdleState::Update(Player* player, float deltaTime)
{
	// アイドル状態での処理
}

void IdleState::Exit(Player* player)
{
	// アイドル状態終了時の処理
}

void IdleState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;
	
	PlayerStateMachine* stateMachine = player->GetStateMachine();
	if (!stateMachine) return;
	
	// 優先度順に状態遷移をチェック
	
	// パリィ
	if (input->IsParrying())
	{
		stateMachine->ChangeState("Parry");
		return;
	}
	
	// 攻撃
	if (input->IsAttacking())
	{
		stateMachine->ChangeState("Attack");
		return;
	}
	
	// 射撃
	if (input->IsShooting())
	{
		stateMachine->ChangeState("Shoot");
		return;
	}
	
	// ダッシュ
	if (input->IsDashing())
	{
		stateMachine->ChangeState("Dash");
		return;
	}
	
	// 移動
	if (input->IsMoving())
	{
		stateMachine->ChangeState("Move");
		return;
	}
}