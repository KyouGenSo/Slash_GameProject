#include "InputHandler.h"
#include "InputAction.h"
#include "InputAxis.h"
#include "Object/Player/Player.h"
#include "Input.h"
#include "Vector2.h"
#include "GlobalVariables.h"

using namespace Tako;

InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::Initialize()
{
  isMoving_ = false;
  isDashing_ = false;
  isAttacking_ = false;
  isShooting_ = false;
  isParrying_ = false;
  isPaused_ = false;
  moveDirection_ = Vector2(0.0f, 0.0f);
}

void InputHandler::Update()
{
  Input* input = Input::GetInstance();

  // 割当は ProjectSettings.json の Input.Actions / Input.Axes（スティックはデッドゾーン適用済み）
  moveDirection_ = input->GetAxis(InputAxis::Move);

  const Vector2 aim = input->GetAxis(InputAxis::Aim);
  const bool hasAimInput = aim.Length() > 0.0f;
  aimDirection_ = hasAimInput ? aim.Normalize() : Vector2(0.0f, 0.0f);

  isMoving_ = moveDirection_.Length() > 0.0f;
  isDashing_ = input->TriggerAction(InputAction::Dash);
  isAttacking_ = input->TriggerAction(InputAction::Attack);
  isShooting_ = hasAimInput;
  isParrying_ = input->TriggerAction(InputAction::Parry);
  isPaused_ = input->TriggerAction(InputAction::Pause);
}

void InputHandler::ResetInputs()
{
    moveDirection_ = Vector2(0.0f, 0.0f);
    isMoving_ = false;
    isDashing_ = false;
    isAttacking_ = false;
    isShooting_ = false;
    isParrying_ = false;
    isPaused_ = false;
}

bool InputHandler::IsMoving() const
{
  return isMoving_;
}

bool InputHandler::IsDashing() const
{
  return isDashing_;
}

bool InputHandler::IsAttacking() const
{
  return isAttacking_;
}

bool InputHandler::IsShooting() const
{
  return isShooting_;
}

bool InputHandler::IsParrying() const
{
  return isParrying_;
}

bool InputHandler::IsPaused() const
{
  return isPaused_;
}

Vector2 InputHandler::GetMoveDirection() const
{
  return moveDirection_;
}

Vector2 InputHandler::GetAimDirection() const
{
  return aimDirection_;
}