#pragma once
#include <memory>
#include "dinput.h"
#include "vector2.h"
#include "Xinput.h"

class Player;

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	void Initialize();
	void Update();
	
	// 入力状態の取得
	bool IsMoving() const;
	bool IsDashing() const;
	bool IsAttacking() const;
	bool IsShooting() const;
	bool IsParrying() const;
	bool IsPaused() const;
	
	// 入力方向の取得
	Vector2 GetMoveDirection() const;

private:
	
	// 入力状態のキャッシュ
	bool isMoving_ = false;
	bool isDashing_ = false;
	bool isAttacking_ = false;
	bool isShooting_ = false;
	bool isParrying_ = false;
	bool isPaused_ = false;
	
	Vector2 moveDirection_;
};