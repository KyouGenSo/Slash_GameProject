#pragma once
#include "PlayerState.h"

/// <summary>
/// 移動状態クラス
/// プレイヤーが歩行・走行している時の状態
/// </summary>
class MoveState : public PlayerState
{
public:
	MoveState() : PlayerState("Walk") {}

	void Enter(Player* player) override;
	void Update(Player* player, float deltaTime) override;
	void Exit(Player* player) override;
	void HandleInput(Player* player) override;
};