#include "Player.h"
#include "Object3d.h"
#include "Input.h"
#include "Camera.h"
#include "State/PlayerStateMachine.h"
#include "State/IdleState.h"
#include "State/MoveState.h"
#include "State/DashState.h"
#include "State/AttackState.h"
#include "State/ShootState.h"
#include "State/ParryState.h"
#include "Input/InputHandler.h"
#include "AABBCollider.h"
#include "MeleeAttackCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include "../Boss/Boss.h"
#include <cmath>

#ifdef _DEBUG
#include "ImGui.h"
#endif

Player::Player()
  : camera_(nullptr)
  , targetAngle_(0.0f)
  , mode_(false)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
  model_ = std::make_unique<Object3d>();
  model_->Initialize();
  model_->SetModel("Player.gltf");

  transform_.translate = Vector3(0.0f, 2.5f, 0.0f);
  transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
  transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

  model_->SetTransform(transform_);

  // State Machineの初期化
  stateMachine_ = std::make_unique<PlayerStateMachine>(this);
  stateMachine_->RegisterState("Idle", std::make_unique<IdleState>());
  stateMachine_->RegisterState("Move", std::make_unique<MoveState>());
  stateMachine_->RegisterState("Dash", std::make_unique<DashState>());
  stateMachine_->RegisterState("Attack", std::make_unique<AttackState>());
  stateMachine_->RegisterState("Shoot", std::make_unique<ShootState>());
  stateMachine_->RegisterState("Parry", std::make_unique<ParryState>());
  stateMachine_->ChangeState("Idle");
  stateMachine_->Initialize();

  // Input Handlerの初期化
  inputHandler_ = std::make_unique<InputHandler>();
  inputHandler_->Initialize();

  // Colliderの初期化
  SetupColliders();
}

void Player::Finalize()
{
  // Colliderを削除
  if (bodyCollider_) {
    CollisionManager::GetInstance()->RemoveCollider(bodyCollider_.get());
  }
  if (meleeAttackCollider_) {
    CollisionManager::GetInstance()->RemoveCollider(meleeAttackCollider_.get());
  }
}

void Player::Update()
{
  float deltaTime = 1.0f / 60.0f; // 60FPSを仮定

  // Inputの更新（StateのHandleInputより前に実行）
  if (inputHandler_)
  {
    inputHandler_->Update();
  }

  // State Machineの更新
  if (stateMachine_)
  {
    stateMachine_->HandleInput();
    stateMachine_->Update(deltaTime);
  }

  // モデルの更新
  model_->SetTransform(transform_);
  model_->Update();

  // 攻撃範囲Colliderの更新
  UpdateAttackCollider();
}

void Player::Draw()
{
  model_->Draw();
}

void Player::Move(float speedMultiplier)
{
  if (!inputHandler_) return;

  Vector2 moveDir = inputHandler_->GetMoveDirection();
  if (moveDir.Length() < 0.1f) return;

  // 3Dベクトルに変換
  velocity_ = { moveDir.x, 0.0f, moveDir.y };
  velocity_ = velocity_.Normalize() * speed_ * speedMultiplier;

  // カメラモードに応じて移動方向を調整
  if (mode_ && camera_)
  {
    Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());
    velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
  }

  // 位置を更新
  transform_.translate += velocity_;

  // 移動方向を向く
  if (velocity_.Length() > 0.01f)
  {
    targetAngle_ = std::atan2(velocity_.x, velocity_.z);
    transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.2f);
  }
}

void Player::MoveToTarget(Boss* target, float deltaTime)
{
  if (!target) return;
  deltaTime;

  // ターゲットの位置取得
  Vector3 targetPos = target->GetTransform().translate;
  Vector3 direction = targetPos - transform_.translate;
  direction.y = 0.0f;

  float distance = direction.Length();
  if (distance > 4.0f) {
    direction = direction.Normalize();

    // ターゲットに向かって移動
    velocity_ = direction * attackMoveSpeed_;
    transform_.translate += velocity_;

    // ターゲットの方向を向く
    targetAngle_ = std::atan2(direction.x, direction.z);
    transform_.rotate.y = Vec3::LerpShortAngle(transform_.rotate.y, targetAngle_, 0.3f);
  }
}

void Player::SetupColliders()
{
  // 本体のCollider
  bodyCollider_ = std::make_unique<AABBCollider>();
  bodyCollider_->SetTransform(&transform_);
  bodyCollider_->SetSize(Vector3(1.5f, 1.5f, 1.5f));
  bodyCollider_->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  bodyCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::kPlayer));
  bodyCollider_->SetOwner(this);

  // 攻撃範囲のCollider
  meleeAttackCollider_ = std::make_unique<MeleeAttackCollider>(this);
  meleeAttackCollider_->SetTransform(&transform_);
  meleeAttackCollider_->SetSize(Vector3(5.f, 2.0f, 25.0f));
  meleeAttackCollider_->SetOffset(Vector3(0.0f, 0.0f, 15.0f));
  meleeAttackCollider_->SetActive(false);

  // CollisionManagerに登録
  CollisionManager* collisionManager = CollisionManager::GetInstance();
  collisionManager->AddCollider(bodyCollider_.get());
  collisionManager->AddCollider(meleeAttackCollider_.get());
}

void Player::UpdateAttackCollider()
{
  if (!meleeAttackCollider_) return;

  // 攻撃状態の時のみ前方に配置
  if (meleeAttackCollider_->IsActive()) {
    // プレイヤーの回転行列を作成（Y軸回転のみ）
    Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(transform_.rotate.y);
    meleeAttackCollider_->SetOrientation(rotationMatrix);
  }
}

void Player::OnMeleeAttackHit(Collider* other)
{
  if (!other) return;

  uint32_t typeID = other->GetTypeID();
  if (typeID == static_cast<uint32_t>(CollisionTypeId::kEnemy)) {
    isAttackHit_ = true;
  }
}

void Player::DrawImGui()
{
#ifdef _DEBUG

  // プレイヤーの状態
  ImGui::Text("=== Player Status ===");
  ImGui::Text("HP: %.1f", hp_);
  ImGui::Text("Position: (%.2f, %.2f, %.2f)", transform_.translate.x, transform_.translate.y, transform_.translate.z);
  ImGui::Text("Speed: %.2f", speed_);

  // State Machine
  ImGui::Separator();
  ImGui::Text("=== State Machine ===");
  if (stateMachine_) {
    PlayerState* currentState = stateMachine_->GetCurrentState();
    if (currentState) {
      ImGui::Text("Current State: %s", currentState->GetName().c_str());

      // AttackStateの場合、詳細情報を表示
      if (currentState->GetName() == "Attack") {
        AttackState* attackState = static_cast<AttackState*>(currentState);
        const char* phaseNames[] = { "SearchTarget", "MoveToTarget", "ExecuteAttack", "Recovery" };
        ImGui::Text("  Attack Phase: %s", phaseNames[attackState->GetPhase()]);
        ImGui::Text("  Search Timer: %.2f / %.2f", attackState->GetSearchTimer(), 0.1f);
        ImGui::Text("  Attack Timer: %.2f", attackState->GetAttackTimer());
        ImGui::Text("  Target Enemy: %s", attackState->GetTargetEnemy() ? "Found" : "None");
      }
    }
  }

  // Input状態
  ImGui::Separator();
  ImGui::Text("=== Input Status ===");
  if (inputHandler_) {
    ImGui::Text("Moving: %s", inputHandler_->IsMoving() ? "Yes" : "No");
    ImGui::Text("Attacking: %s", inputHandler_->IsAttacking() ? "Yes" : "No");
    ImGui::Text("Shooting: %s", inputHandler_->IsShooting() ? "Yes" : "No");
    ImGui::Text("Dashing: %s", inputHandler_->IsDashing() ? "Yes" : "No");
    ImGui::Text("Parrying: %s", inputHandler_->IsParrying() ? "Yes" : "No");
  }

  // Collider状態
  ImGui::Separator();
  ImGui::Text("=== Colliders ===");
  if (bodyCollider_) {
    ImGui::Text("Body Collider: %s", bodyCollider_->IsActive() ? "Active" : "Inactive");
    Vector3 center = bodyCollider_->GetCenter();
    ImGui::Text("Body Center: (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
    Vector3 size = bodyCollider_->GetSize();
    ImGui::Text("Body Size: (%.2f, %.2f, %.2f)", size.x, size.y, size.z);
  }
  if (meleeAttackCollider_) {
    ImGui::Text("Attack Collider: %s", meleeAttackCollider_->IsActive() ? "Active" : "Inactive");
    Boss* detectedEnemy = meleeAttackCollider_->GetDetectedEnemy();
    ImGui::Text("Detected Enemy: %s", detectedEnemy ? "Yes" : "No");
    ImGui::Text("Collision Count: %d", meleeAttackCollider_->GetCollisionCount());
    // 攻撃範囲のオフセット情報
    Vector3 offset = meleeAttackCollider_->GetOffset();
    ImGui::Text("Attack Offset: (%.2f, %.2f, %.2f)", offset.x, offset.y, offset.z);
    Vector3 size = meleeAttackCollider_->GetSize();
    ImGui::Text("Attack Size: (%.2f, %.2f, %.2f)", size.x, size.y, size.z);
  }

  if (ImGui::Button("Model Debug Info")) {
    isDisModelDebugInfo_ = !isDisModelDebugInfo_;
  }

  if (isDisModelDebugInfo_) {
    model_->DrawImGui();
  }

#endif
}