#include <numbers>

#include "Player.h"
#include "BaseScene.h"
#include "MeshComponent.h"
#include "Script/Bullet.h"
#include "Object3d.h"
#include "UILayoutComponent.h"
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
#include "OBBCollider.h"
#include "../../Collision/MeleeAttackCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeId.h"
#include "../Boss/Boss.h"
#include "Settings/PlayerSettings.h"
#include "../../Common/GameConst.h"
#include "../../Common/DamageFeedback.h"
#include "FrameTimer.h"
#include "EmitterManager.h"
#include "ForceFieldManager.h"
#include "../../Common/ForceFieldAffectMask.h"
#include "EnginePaths.h"
#include "ColorFunc.h"

#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

Player::Player()
    : camera_(nullptr)
    , targetAngle_(0.0f)
    , mode_(false)
{
    ClearDynamicBounds();
}

Player::~Player()
{
}

void Player::BeginPlay()
{
    if (UILayoutComponent* layout = GetOwner()->GetComponent<UILayoutComponent>()) {
        hpBar_.Bind(&layout->GetLayout());
    }

    stateMachine_ = std::make_unique<PlayerStateMachine>(this);
    stateMachine_->RegisterState("Idle", std::make_unique<IdleState>());
    stateMachine_->RegisterState("Move", std::make_unique<MoveState>());
    stateMachine_->RegisterState("Dash", std::make_unique<DashState>());
    stateMachine_->RegisterState("Attack", std::make_unique<AttackState>());
    stateMachine_->RegisterState("Shoot", std::make_unique<ShootState>());
    stateMachine_->RegisterState("Parry", std::make_unique<ParryState>());
    stateMachine_->ChangeState("Idle");
    stateMachine_->Initialize();

    SetupColliders();
    SetAttackBlockVisible(false);
}

void Player::EndPlay()
{
    if (meleeAttackCollider_) {
        CollisionManager::GetInstance()->RemoveCollider(meleeAttackCollider_.get());
    }
}

void Player::Tick([[maybe_unused]] float deltaTime)
{
    Update();
}

Tako::Object3d* Player::GetModel() const
{
    MeshComponent* mesh = GetOwner()->GetComponent<MeshComponent>();
    return mesh ? mesh->GetObject3d() : nullptr;
}

void Player::SetAttackBlockVisible(bool visible)
{
    Tako::Actor*   block = GetAttackBlock();
    MeshComponent* mesh  = block ? block->GetComponent<MeshComponent>() : nullptr;
    if (mesh) {
        mesh->SetVisible(visible);
    }
}

bool Player::IsAttackBlockVisible() const
{
    const Tako::Actor*   block = GetAttackBlock();
    const MeshComponent* mesh  = block ? block->GetComponent<MeshComponent>() : nullptr;
    return mesh && mesh->IsVisible();
}

void Player::Update()
{
    if (isPause_) return;

    SyncSettings();

    float deltaTime = FrameTimer::GetInstance()->GetDeltaTime();

    UpdateCombat(deltaTime);
    UpdateStateMachine(deltaTime);
    UpdateVisuals(deltaTime);
    UpdateCollider();
    UpdatePhysics(deltaTime);
    UpdateTransform();
}

void Player::SyncSettings()
{
    const PlayerSettings& settings = Tako::GameSettings::Get<PlayerSettings>();
    speed_ = settings.speed;
    attackMinDist_ = settings.attackStartDistance;
    attackMoveRotationLerp_ = settings.attackMoveRotationLerp;
    bossLookatLerp_ = settings.bossLookatLerp;
    attackMoveSpeed_ = settings.attackMoveSpeed;
}

void Player::UpdateCombat(float deltaTime)
{
    parryCooldown_.Update(deltaTime);
    dashCooldown_.Update(deltaTime);

    if (hp_ <= 0.0f) isDead_ = true;

    hpBar_.Update(hp_, kMaxHp);
}

void Player::UpdateStateMachine(float deltaTime)
{
    if (stateMachine_) {
        stateMachine_->HandleInput();
        stateMachine_->Update(deltaTime);
    }

    // フェーズ2時とパリィ中はボスの方向を向く
    if ((targetEnemy_ && targetEnemy_->GetPhase() == 2) || IsParrying()) {
        LookAtBoss(deltaTime);
    }
}

void Player::UpdateTransform()
{
    // 静的制限と動的制限の交差をとった実効制限
    DynamicBoundary effectiveBounds;
    effectiveBounds.Set(
        std::max<float>(GameConst::kStageXMin, dynamicBounds_.xMin),
        std::min<float>(GameConst::kStageXMax, dynamicBounds_.xMax),
        std::max<float>(GameConst::kStageZMin, dynamicBounds_.zMin),
        std::min<float>(GameConst::kStageZMax, dynamicBounds_.zMax));

    GetOwner()->GetTransform().translate = effectiveBounds.Clamp(GetOwner()->GetTransform().translate);
}

void Player::UpdatePhysics(float deltaTime)
{
    if (forceFieldManager_) {
        const Vector3 force = forceFieldManager_->EvaluateForceAt(
            GetOwner()->GetTransform().translate, GameForceField::AffectPlayer);
        externalVelocity_ += force * deltaTime; // 加速度 → 速度
    }
    GetOwner()->GetTransform().translate.x += externalVelocity_.x * deltaTime;
    GetOwner()->GetTransform().translate.z += externalVelocity_.z * deltaTime;
    externalVelocity_ *= FrameRate::Damping(kExternalVelocityDamping, deltaTime);
}

void Player::UpdateVisuals(float deltaTime)
{
    hitFlashEffect_.Update(deltaTime, GetModel());

    shakeEffect_.Update(deltaTime);

    // Actor の位置は変えず、見た目だけを揺らす
    if (MeshComponent* mesh = GetOwner()->GetComponent<MeshComponent>()) {
        Transform offset = mesh->GetOffset();
        offset.translate = shakeEffect_.GetOffset();
        mesh->SetOffset(offset);
    }
}

void Player::Move(float deltaTime, float speedMultiplier, bool isApplyDirCalulate)
{
    if (!inputHandlerPtr_) return;

    const PlayerSettings& settings = Tako::GameSettings::Get<PlayerSettings>();
    float deadzone = settings.moveInputDeadzone;
    float rotationLerpSpeed = settings.rotationLerpSpeed;

    Vector2 moveDir = inputHandlerPtr_->GetMoveDirection();
    if (moveDir.Length() < deadzone) return;

    velocity_ = { moveDir.x, 0.0f, moveDir.y };
    velocity_ = velocity_.Normalize() * speed_ * speedMultiplier;

    // カメラ相対の移動にするため入力をカメラ Y 回転で変換
    if (mode_ && camera_) {
        Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(camera_->GetRotateY());
        velocity_ = Mat4x4::TransformNormal(rotationMatrix, velocity_);
    }

    // velocity_ は 60fps の 1 フレームあたりの移動量
    GetOwner()->GetTransform().translate += velocity_ * FrameRate::Frames(deltaTime);

    // 移動方向を向く
    if (velocity_.Length() > kVelocityEpsilon && isApplyDirCalulate) {
        targetAngle_ = std::atan2(velocity_.x, velocity_.z);
        GetOwner()->GetTransform().rotate.y = Vec3::LerpShortAngle(GetOwner()->GetTransform().rotate.y, targetAngle_, FrameRate::LerpFactor(rotationLerpSpeed, deltaTime));
    }
}

void Player::MoveToTarget(const Vector3& targetPos, float deltaTime, float stopDistance)
{
    if (!attackMover_.IsInitialized()) {
        Vector3 startPos = GetOwner()->GetTransform().translate;
        Vector3 toTarget = targetPos - startPos;
        toTarget.y = 0.0f;
        float distance = toTarget.Length();

        if (distance > stopDistance && distance > kDirectionEpsilon) {
            Vector3 direction = toTarget.Normalize();

            // 目標位置 = targetPos から stopDistance 手前
            float moveDistance = distance - stopDistance;
            Vector3 moveTargetPos = startPos + direction * moveDistance;
            moveTargetPos.y = startPos.y;

            attackMover_.InitializeWithSpeed(startPos, moveTargetPos, attackMoveSpeed_);

            targetAngle_ = std::atan2(direction.x, direction.z);
        }
        else {
            // 既に攻撃範囲内（移動不要）
            attackMover_.Initialize(startPos, startPos, 0.0f);
        }
    }

    GetOwner()->GetTransform().translate = attackMover_.Update(deltaTime);

    GetOwner()->GetTransform().rotate.y = Vec3::LerpShortAngle(GetOwner()->GetTransform().rotate.y, targetAngle_, FrameRate::LerpFactor(attackMoveRotationLerp_, deltaTime));
}

void Player::ResetMoveToTarget()
{
    attackMover_.Reset();
}

bool Player::HasReachedTarget() const
{
    return attackMover_.HasReached();
}

void Player::SetupColliders()
{
    const PlayerSettings& settings = Tako::GameSettings::Get<PlayerSettings>();

    // 攻撃範囲
    meleeAttackCollider_ = std::make_unique<MeleeAttackCollider>(this);
    meleeAttackCollider_->SetTransform(&GetOwner()->GetTransform());
    meleeAttackCollider_->SetSize(settings.meleeColliderSize);
    meleeAttackCollider_->SetOffset(Vector3(0.0f, 0.0f, settings.meleeColliderOffsetZ));
    meleeAttackCollider_->SetActive(false);

    CollisionManager::GetInstance()->AddCollider(meleeAttackCollider_.get());
}

void Player::UpdateCollider()
{
    const PlayerSettings& settings = Tako::GameSettings::Get<PlayerSettings>();

    if (!meleeAttackCollider_) return;

    // 攻撃中のみ前方に配置
    if (meleeAttackCollider_->IsActive()) {
        Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(GetOwner()->GetTransform().rotate.y);
        meleeAttackCollider_->SetOrientation(rotationMatrix);

        meleeAttackCollider_->SetSize(settings.meleeColliderSize);
        meleeAttackCollider_->SetOffset(Vector3(0.0f, 0.0f, settings.meleeColliderOffsetZ));
    }
}

void Player::LookAtBoss(float deltaTime)
{
    Vector3 toTarget = targetEnemy_->GetTransform().translate - GetOwner()->GetTransform().translate;
    toTarget.y = 0.0f;  // Y 軸は無視

    if (toTarget.Length() < 0.01f) return;

    float targetAngle = std::atan2(toTarget.x, toTarget.z);

    GetOwner()->GetTransform().rotate.y = Vec3::LerpShortAngle(GetOwner()->GetTransform().rotate.y, targetAngle, FrameRate::LerpFactor(bossLookatLerp_, deltaTime));
}

void Player::OnHit(float damage)
{
    if (IsInvincible()) return;

    hp_ -= damage;
    hp_ = std::max<float>(hp_, 0.0f);

    hitFlashEffect_.Start(Vector4(1.0f, 0.0f, 0.0f, 1.0f), 0.05f);

    shakeEffect_.Start(0.5f);

    // カメラシェイク、振動、Vignette
    DamageFeedback::TriggerHitFeedback();
}

#ifdef _DEBUG
bool Player::DrawCustomImGui()
{
    static int selectedTab = 0;

    if (ImGui::BeginTabBar("PlayerDebugTabs", ImGuiTabBarFlags_None)) {

        // ========== General タブ ==========
        if (ImGui::BeginTabItem("General")) {
            selectedTab = 0;

            // HP
            ImGui::Text("Health");
            ImGui::SliderFloat("HP", &hp_, 0.0f, 200.0f, "%.1f");
            ImGui::ProgressBar(hp_ / 100.0f, ImVec2(-1, 0), "");
            ImGui::Checkbox("Invincible", &isInvincible_);

            ImGui::Separator();

            // Transform
            if (ImGui::TreeNode("Transform")) {
                ImGui::DragFloat3("Position", &GetOwner()->GetTransform().translate.x, 0.1f);
                ImGui::DragFloat3("Rotation", &GetOwner()->GetTransform().rotate.x, 0.01f);
                ImGui::DragFloat3("Scale", &GetOwner()->GetTransform().scale.x, 0.01f);
                ImGui::TreePop();
            }

            // Speed
            ImGui::Separator();
            ImGui::SliderFloat("Move Speed", &speed_, 0.0f, 2.0f);

            ImGui::EndTabItem();
        }

        // ========== States タブ ==========
        if (ImGui::BeginTabItem("States")) {
            selectedTab = 1;

            if (stateMachine_) {
                PlayerState* currentState = stateMachine_->GetCurrentState();
                if (currentState) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Active State: %s", currentState->GetName().c_str());
                }

                ImGui::Separator();

                if (ImGui::TreeNode("All States Details")) {
                    static std::string selectedStateName = "Idle";

                    auto stateNames = stateMachine_->GetAllStateNames();
                    if (ImGui::BeginCombo("Select State", selectedStateName.c_str())) {
                        for (const auto& name : stateNames) {
                            bool isSelected = (selectedStateName == name);

                            std::string displayName = name;
                            if (currentState && currentState->GetName() == name) {
                                displayName = name + " [ACTIVE]";
                            }

                            if (ImGui::Selectable(displayName.c_str(), isSelected)) {
                                selectedStateName = name;
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    PlayerState* selectedState = stateMachine_->GetState(selectedStateName);
                    if (selectedState) {
                        // 現在のステートは緑、他は青
                        if (currentState && currentState->GetName() == selectedStateName) {
                            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.4f, 0.1f, 1.0f));
                        }
                        else {
                            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
                        }

                        if (ImGui::CollapsingHeader((selectedStateName + " State Details").c_str(),
                            ImGuiTreeNodeFlags_DefaultOpen)) {
                            selectedState->DrawImGui(this);
                        }
                        ImGui::PopStyleColor();
                    }

                    ImGui::TreePop();
                }

                ImGui::Separator();

                if (ImGui::TreeNode("Manual State Change")) {
                    if (ImGui::Button("Idle")) stateMachine_->ChangeState("Idle");
                    ImGui::SameLine();
                    if (ImGui::Button("Move")) stateMachine_->ChangeState("Move");
                    ImGui::SameLine();
                    if (ImGui::Button("Dash")) stateMachine_->ChangeState("Dash");

                    if (ImGui::Button("Attack")) stateMachine_->ChangeState("Attack");
                    ImGui::SameLine();
                    if (ImGui::Button("Shoot")) stateMachine_->ChangeState("Shoot");
                    ImGui::SameLine();
                    if (ImGui::Button("Parry")) stateMachine_->ChangeState("Parry");

                    ImGui::TreePop();
                }
            }

            ImGui::EndTabItem();
        }

        // ========== Combat タブ ==========
        if (ImGui::BeginTabItem("Combat")) {
            selectedTab = 2;

            // Attack Collider
            if (ImGui::TreeNode("Attack Collider")) {
                if (meleeAttackCollider_) {
                    bool isActive = meleeAttackCollider_->IsActive();
                    ImGui::Text("Active: %s", isActive ? "YES" : "NO");

                    Vector3 actualCenter = meleeAttackCollider_->GetCenter();
                    ImGui::Text("Actual Center: (%.2f, %.2f, %.2f)",
                        actualCenter.x, actualCenter.y, actualCenter.z);

                    Vector3 offset = meleeAttackCollider_->GetOffset();
                    if (ImGui::DragFloat3("Offset", &offset.x, 0.1f)) {
                        meleeAttackCollider_->SetOffset(offset);
                    }

                    Vector3 size = meleeAttackCollider_->GetSize();
                    if (ImGui::DragFloat3("Size", &size.x, 0.1f, 0.1f, 50.0f)) {
                        meleeAttackCollider_->SetSize(size);
                    }

                    ImGui::Separator();
                    Boss* detectedEnemy = meleeAttackCollider_->GetDetectedEnemy();
                    if (detectedEnemy) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Enemy Detected!");
                    }
                    else {
                        ImGui::Text("No Enemy Detected");
                    }
                    ImGui::Text("Collision Count: %d", meleeAttackCollider_->GetCollisionCount());
                }
                ImGui::TreePop();
            }

            // Combat Parameters
            if (ImGui::TreeNode("Combat Parameters")) {
                ImGui::SliderFloat("Attack Move Speed", &attackMoveSpeed_, 0.5f, 10.0f);
                ImGui::SliderFloat("Attack Start Distance", &attackMinDist_, 1.0f, 10.0f);
                ImGui::SliderFloat("Attack Move Rotation Lerp", &attackMoveRotationLerp_, 0.01f, 1.0f);
                ImGui::SliderFloat("Boss Lookat Lerp", &bossLookatLerp_, 0.01f, 2.0f);
                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        // ========== Physics タブ ==========
        if (ImGui::BeginTabItem("Physics")) {
            selectedTab = 3;

            // Velocity
            ImGui::Text("Velocity");
            ImGui::Text("X: %.3f, Y: %.3f, Z: %.3f", velocity_.x, velocity_.y, velocity_.z);
            ImGui::Text("Magnitude: %.3f", velocity_.Length());

            // Velocity Graph
            static float velocityHistory[100] = { 0 };
            static int historyOffset = 0;
            velocityHistory[historyOffset] = velocity_.Length();
            historyOffset = (historyOffset + 1) % 100;
            ImGui::PlotLines("Velocity History", velocityHistory, 100, historyOffset, nullptr, 0.0f, 20.0f, ImVec2(0, 80));

            ImGui::Separator();

            // Target Angle
            ImGui::Text("Target Angle: %.2f degrees", targetAngle_ * 180.0f / std::numbers::pi_v<float>);
            ImGui::Text("Current Y Rotation: %.2f degrees", GetOwner()->GetTransform().rotate.y * 180.0f / std::numbers::pi_v<float>);

            ImGui::Separator();

            // Movement Direction Visualization
            if (ImGui::TreeNode("Movement Visualization")) {
                float angle = GetOwner()->GetTransform().rotate.y;
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 center = ImGui::GetCursorScreenPos();
                center.x += 50;
                center.y += 50;

                draw_list->AddCircle(center, 40, IM_COL32(100, 100, 100, 255));

                float arrowX = cosf(angle) * 35;
                float arrowY = sinf(angle) * 35;
                draw_list->AddLine(center,
                    ImVec2(center.x + arrowX, center.y - arrowY),
                    IM_COL32(0, 255, 0, 255), 3.0f);

                ImGui::Dummy(ImVec2(100, 100));
                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        // ========== Debug タブ ==========
        if (ImGui::BeginTabItem("Debug")) {
            selectedTab = 4;

            // Input Status
            if (ImGui::TreeNode("Input Status")) {
                if (inputHandlerPtr_) {
                    bool moving = inputHandlerPtr_->IsMoving();
                    bool attacking = inputHandlerPtr_->IsAttacking();
                    bool shooting = inputHandlerPtr_->IsShooting();
                    bool dashing = inputHandlerPtr_->IsDashing();
                    bool parrying = inputHandlerPtr_->IsParrying();

                    ImGui::Text("Moving: %s", moving ? "✓" : "✗");
                    ImGui::Text("Attacking: %s", attacking ? "✓" : "✗");
                    ImGui::Text("Shooting: %s", shooting ? "✓" : "✗");
                    ImGui::Text("Dashing: %s", dashing ? "✓" : "✗");
                    ImGui::Text("Parrying: %s", parrying ? "✓" : "✗");
                }
                ImGui::TreePop();
            }

            // Model Debug
            ImGui::Separator();
            if (ImGui::Button("Show Model Debug Info")) isDisModelDebugInfo_ = !isDisModelDebugInfo_;

            if (ImGui::TreeNode("Animation Control")) {
                ImGui::Text("TODO: Animation system integration");
                ImGui::TreePop();
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (isDisModelDebugInfo_) {
        if (Tako::Object3d* model = GetModel()) {
            model->DrawImGui();
        }
    }

    return false;
}
#endif

bool Player::CanShoot() const
{
    if (isDead_) return false;
    if (targetEnemy_->GetPhase() == 2) return false; // フェーズ2では射撃不可

    return true;
}

void Player::SetDynamicBounds(float xMin, float xMax, float zMin, float zMax)
{
    dynamicBounds_.Set(xMin, xMax, zMin, zMax);
}

void Player::SetDynamicBoundsFromCenter(const Vector3& center, float xRange, float zRange)
{
    dynamicBounds_.SetFromCenter(center, xRange, zRange);
}

void Player::ClearDynamicBounds()
{
    dynamicBounds_.Clear();
}

void Player::RequestBulletSpawn(const Vector3& position, const Vector3& velocity)
{
    Bullet::Spawn(*GetOwner()->GetScene(), "PlayerBullet", position, velocity);
}

bool Player::IsParrying() const
{
    if (!stateMachine_ || !stateMachine_->GetCurrentState()) return false;
    return stateMachine_->GetCurrentState()->GetName() == "Parry";
}

bool Player::IsShooting() const
{
    if (!stateMachine_ || !stateMachine_->GetCurrentState()) return false;
    return stateMachine_->GetCurrentState()->GetName() == "Shoot";
}

Vector3 Player::GetAimDirection() const
{
    ShootState* shootState = stateMachine_ ? static_cast<ShootState*>(stateMachine_->GetState("Shoot")) : nullptr;
    if (!shootState) return Vector3(0.0f, 0.0f, 0.0f);
    return shootState->GetAimDirection();
}

bool Player::CanParry() const
{
    return parryCooldown_.IsReady();
}

void Player::StartParryCooldown()
{
    float cooldown = Tako::GameSettings::Get<PlayerSettings>().parry.cooldown;
    if (cooldown <= 0.0f) {
        cooldown = 1.0f;  // デフォルト1秒
    }
    parryCooldown_.Start(cooldown);
}

bool Player::CanDash() const
{
    return dashCooldown_.IsReady();
}

void Player::StartDashCooldown()
{
    float cooldown = Tako::GameSettings::Get<PlayerSettings>().dash.cooldown;
    if (cooldown <= 0.0f) {
        cooldown = 0.5f;  // デフォルト0.5秒
    }
    dashCooldown_.Start(cooldown);
}

void Player::OnParrySuccess()
{
    float healAmount = Tako::GameSettings::Get<PlayerSettings>().parry.successHealAmount;
    hp_ = std::min<float>(hp_ + healAmount, kMaxHp);

    Vector3 effectPos = GetFrontPosition(2.0f);
    DamageFeedback::TriggerParryFeedback(effectPos, emitterManager_);
}

Tako::Vector3 Player::GetFrontPosition(float offset) const
{
    Vector3 forward;
    forward.x = std::sin(GetOwner()->GetTransform().rotate.y);
    forward.y = 0.0f;
    forward.z = std::cos(GetOwner()->GetTransform().rotate.y);
    return GetOwner()->GetTransform().translate + forward * offset;
}