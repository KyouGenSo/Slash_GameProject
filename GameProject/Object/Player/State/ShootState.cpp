#include "ShootState.h"
#include "PlayerStateMachine.h"
#include "../Player.h"
#include "Input/InputHandler.h"
#include "Camera.h"
#include "Matrix4x4.h"

void ShootState::Enter(Player* player)
{
	// 射撃アニメーションを再生
  // TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Shoot");
	
	fireRateTimer_ = 0.0f;
}

void ShootState::Update(Player* player, float deltaTime)
{
	// 発射レートタイマーの更新
	if (fireRateTimer_ > 0.0f)
	{
		fireRateTimer_ -= deltaTime;
	}
	
	// エイム方向の計算
	CalculateAimDirection(player);
	
	// 射撃可能なら発射
	if (fireRateTimer_ <= 0.0f)
	{
		Fire(player);
		fireRateTimer_ = fireRate_;
	}
	
	// 移動処理
	player->Move(0.5f);
}

void ShootState::Exit(Player* player)
{
	fireRateTimer_ = 0.0f;
}

void ShootState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;
	
	// 射撃ボタンが離されたら元の状態に戻る
	if (!input->IsShooting())
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			if (input->IsMoving())
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

void ShootState::CalculateAimDirection(Player* player)
{
	Camera* camera = player->GetCamera();
	if (camera)
	{
		// カメラのビュー行列から前方ベクトルを取得
		Matrix4x4 viewMatrix = camera->GetViewMatrix();
		Matrix4x4 invViewMatrix = Mat4x4::Inverse(viewMatrix);
		aimDirection_ = Vector3(-invViewMatrix.m[2][0], -invViewMatrix.m[2][1], -invViewMatrix.m[2][2]).Normalize();
	}
}

void ShootState::Fire(Player* player)
{
	// 弾を発射する処理
	// TODO: 弾オブジェクトの生成と発射処理を実装
}