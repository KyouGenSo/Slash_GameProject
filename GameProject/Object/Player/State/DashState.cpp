#include "DashState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "Input/InputHandler.h"

void DashState::Enter(Player* player)
{
	// ダッシュアニメーションを再生
  // TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Dash");
	
	timer_ = 0.0f;
}

void DashState::Update(Player* player, float deltaTime)
{
	timer_ += deltaTime;
	
	// ダッシュ移動処理
	player->Move(speed_);
	
	// ダッシュ時間が終了したら元の状態に戻る
	if (timer_ >= duration_)
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			// 移動入力があればWalk、なければIdleに遷移
			InputHandler* input = player->GetInputHandler();
			if (input && input->IsMoving())
			{
				stateMachine->ChangeState("Move");
			}
			else
			{
				stateMachine->ChangeState("Idle");
			}
		}
	}
}

void DashState::Exit(Player* player)
{
	timer_ = 0.0f;
}

void DashState::HandleInput(Player* player)
{
	// ダッシュ中は入力を受け付けない
}