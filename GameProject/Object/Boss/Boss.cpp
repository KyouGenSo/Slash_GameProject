#include "Boss.h"
#include "BaseScene.h"
#include "Script/Bullet.h"
#include "BehaviorTreeComponent.h"
#include "MeshComponent.h"
#include "UILayoutComponent.h"
#include "Boss.h"
#include <algorithm>
#include <filesystem>
#include "Object3d.h"
#include "Model.h"
#include "PrimitiveBuilder.h"
#include "OBBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeId.h"
#include "../../Collision/BossMeleeAttackCollider.h"
#include "WinApp.h"
#include "EnginePaths.h"
#include "ColorFunc.h"
#include "Settings/BossSettings.h"
#include "Settings/GameplaySettings.h"
#include "EmitterManager.h"
#include "FrameTimer.h"
#include "State/BossStateMachine.h"
#include "State/BossNormalState.h"
#include "State/BossStunnedState.h"
#include "State/BossRetreatingState.h"
#include "State/BossPhaseTransitionStunState.h"
#include "State/BossDeadState.h"
#include "../../CameraSystem/CameraManager.h"
#include "../Player/Player.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

namespace {

    MeshComponent* MeshOf(const Tako::Actor* actor) {
        return actor ? actor->GetComponent<MeshComponent>() : nullptr;
    }

    void SetMeshVisible(const Tako::Actor* actor, bool visible) {
        if (MeshComponent* mesh = MeshOf(actor)) {
            mesh->SetVisible(visible);
        }
    }

}

Boss::Boss()
{}

Boss::~Boss()
{}

void Boss::BeginPlay()
{
    Tako::BaseScene* scene = GetOwner()->GetScene();
    emitterManager_    = &scene->GetEmitterManager();
    forceFieldManager_ = &scene->GetForceFieldManager();

    InitializeModel();
    InitializeHealth();
    InitializeColliders();
    ApplyEffectSettings();
    InitializeAI();
    InitializeStateMachine();

    // ボスのモデルとリングの形に沿って出すエミッターは、形（Object3d）を結び付けるのでここで作る
    InitializeAuraEmitter();
    InitializeBodyParticleEmitter();
    InitializeRingShockwaveEmitter();
}

void Boss::InitializeStateMachine()
{
    stateMachine_ = std::make_unique<BossStateMachine>(this);
    stateMachine_->RegisterState("Normal", std::make_unique<BossNormalState>());
    stateMachine_->RegisterState("Stunned", std::make_unique<BossStunnedState>());
    stateMachine_->RegisterState("Retreating", std::make_unique<BossRetreatingState>());
    stateMachine_->RegisterState("PhaseTransitionStun", std::make_unique<BossPhaseTransitionStunState>());
    stateMachine_->RegisterState("Dead", std::make_unique<BossDeadState>());
    stateMachine_->ChangeState("Normal");
}

void Boss::InitializeModel()
{
    // 半径 1 の単位球。大きさは Actor の scale で変える
    if (MeshComponent* sphere = MeshOf(GetOwner()->Resolve(repelShockwaveSphere_))) {
        sphere->SetGeneratedModel(PrimitiveBuilder::CreateSphere({ .radius = 1.0f, .lonDiv = 32, .latDiv = 16 }));
    }
    // 外半径 1 の単位半円リング（弧の中心がローカルの +Z）。大きさは Actor の scale で変える
    if (MeshComponent* ring = MeshOf(GetRingShockwaveActor())) {
        ring->SetGeneratedModel(PrimitiveBuilder::CreateRing({
            .innerRadius = 1.0f - kRingShockwaveWidthFraction,
            .outerRadius = 1.0f,
            .segments = 48,
            .ringSeg = 1,
            .startAngleDeg = 0.0f,
            .sweepAngleDeg = 180.0f }));
    }
    SetMeleeAttackBlockVisible(false);
    SetRepelShockwaveSphereVisible(false);
    SetRingShockwaveVisible(false);
}

void Boss::InitializeHealth()
{
    if (UILayoutComponent* layout = GetOwner()->GetComponent<UILayoutComponent>()) {
        hpBar_.Bind(&layout->GetLayout());
    }

    phaseManager_.Initialize(kMaxHp, kPhase2Threshold, kPhase2InitialHp);
}

void Boss::InitializeColliders()
{
    const BossSettings& settings = Tako::GameSettings::Get<BossSettings>();

    // 近接攻撃コライダー（本体は同じ Actor の ColliderComponent）
    const BossSettings::MeleeCollider& melee = settings.melee;
    meleeAttackCollider_ = std::make_unique<BossMeleeAttackCollider>(this);
    meleeAttackCollider_->SetTransform(&GetOwner()->GetTransform());
    meleeAttackCollider_->SetSize(melee.size);
    meleeAttackCollider_->SetOffset(Vector3(0.0f, 0.0f, melee.offsetZ));
    meleeAttackCollider_->SetOwner(this);
    CollisionManager::GetInstance()->AddCollider(meleeAttackCollider_.get());
}

void Boss::ApplyEffectSettings()
{
    const ShakeSettings& shake = Tako::GameSettings::Get<BossSettings>().shake;
    shakeEffect_.SetDefaultDuration(shake.duration);
    shakeEffect_.SetDefaultIntensity(shake.intensity);
}

void Boss::InitializeAuraEmitter()
{
    if (!emitterManager_ || !GetModel()) {
        return;
    }

    // 二重登録防止
    if (emitterManager_->HasEmitter(auraEmitterName_)) {
        emitterManager_->RemoveEmitter(auraEmitterName_);
    }

    emitterManager_->LoadPreset(auraEmitterName_, auraEmitterName_, GetModel());
    emitterManager_->LoadPreset(darkAuraEmitterName_, darkAuraEmitterName_, GetModel());

    emitterManager_->SetEmitterActive(auraEmitterName_, !isInRecovery_ && !IsStunned() && !isTeleporting_);
    emitterManager_->SetEmitterActive(darkAuraEmitterName_, !isInRecovery_ && !IsStunned() && !isTeleporting_);
}

void Boss::SetAuraEmitterActive(bool active)
{
    if (!emitterManager_) return;
    emitterManager_->SetEmitterActive(auraEmitterName_, active);
    emitterManager_->SetEmitterActive(darkAuraEmitterName_, active);
}

void Boss::InitializeBodyParticleEmitter()
{
    if (!emitterManager_ || !GetModel()) {
        return;
    }

    // 二重登録防止
    if (emitterManager_->HasEmitter(bodyParticleEmitterName_)) {
        emitterManager_->RemoveEmitter(bodyParticleEmitterName_);
    }

    emitterManager_->LoadPreset(bodyParticleEmitterName_, bodyParticleEmitterName_, GetModel());

    emitterManager_->SetEmitterActive(bodyParticleEmitterName_, false);
}

void Boss::SetBodyParticleEmitterActive(bool active)
{
    if (!emitterManager_) return;
    emitterManager_->SetEmitterActive(bodyParticleEmitterName_, active);
}

void Boss::InitializeRingShockwaveEmitter()
{
    if (!emitterManager_ || !GetRingShockwaveModel()) {
        return;
    }

    // 二重登録防止
    if (emitterManager_->HasEmitter(ringShockwaveEmitterName_)) {
        emitterManager_->RemoveEmitter(ringShockwaveEmitterName_);
    }

    emitterManager_->LoadPreset(ringShockwaveEmitterName_, ringShockwaveEmitterName_, GetRingShockwaveModel());

    emitterManager_->SetEmitterActive(ringShockwaveEmitterName_, false);
}

void Boss::InitializeAI()
{
    // ツリーは BehaviorTreeComponent が読む。ノードが使う相手を Blackboard に入れる
    if (Tako::BehaviorTree* tree = GetBehaviorTree()) {
        Tako::BTBlackboard* blackboard = tree->GetBlackboard();
        blackboard->SetPtr<Boss>("boss", this);
        blackboard->SetPtr<Player>("player", player_);
    }
}

void Boss::EndPlay()
{
    if (meleeAttackCollider_) {
        CollisionManager::GetInstance()->RemoveCollider(meleeAttackCollider_.get());
    }

    if (emitterManager_) {
        if (emitterManager_->HasEmitter(auraEmitterName_)) {
            emitterManager_->RemoveEmitter(auraEmitterName_);
        }
        if (emitterManager_->HasEmitter(darkAuraEmitterName_)) {
            emitterManager_->RemoveEmitter(darkAuraEmitterName_);
        }
        if (emitterManager_->HasEmitter(bodyParticleEmitterName_)) {
            emitterManager_->RemoveEmitter(bodyParticleEmitterName_);
        }
        if (emitterManager_->HasEmitter(ringShockwaveEmitterName_)) {
            emitterManager_->RemoveEmitter(ringShockwaveEmitterName_);
        }
    }
}

void Boss::Tick(float deltaTime)
{
    Update(deltaTime);
}

Tako::Object3d* Boss::GetModel() const
{
    MeshComponent* mesh = GetOwner()->GetComponent<MeshComponent>();
    return mesh ? mesh->GetObject3d() : nullptr;
}

Tako::Object3d* Boss::GetRingShockwaveModel() const
{
    MeshComponent* mesh = MeshOf(GetRingShockwaveActor());
    return mesh ? mesh->GetObject3d() : nullptr;
}

Tako::BehaviorTree* Boss::GetBehaviorTree() const
{
    Tako::BehaviorTreeComponent* component = GetOwner()->GetComponent<Tako::BehaviorTreeComponent>();
    return component ? &component->GetTree() : nullptr;
}

void Boss::SetMeleeAttackBlockVisible(bool visible)
{
    SetMeshVisible(GetMeleeAttackBlock(), visible);
}

bool Boss::IsMeleeAttackBlockVisible() const
{
    const MeshComponent* mesh = MeshOf(GetMeleeAttackBlock());
    return mesh && mesh->IsVisible();
}

void Boss::SetRepelShockwaveSphereVisible(bool visible)
{
    SetMeshVisible(GetOwner()->Resolve(repelShockwaveSphere_), visible);
}

void Boss::SetRingShockwaveVisible(bool visible)
{
    SetMeshVisible(GetRingShockwaveActor(), visible);
}

void Boss::Update(float deltaTime)
{
    hpBar_.UpdateDual(hp_, kMaxHp, kPhase2Threshold, phaseManager_.GetPhase());

    phaseManager_.Update(hp_);

    if (phaseManager_.IsDead() && stateMachine_->GetCurrentStateName() != "Dead") {
        stateMachine_->ChangeState("Dead");
    }

    // ステートマシンが全てを駆動（Normal 内で BT が動く）
    if (!isPause_) {
        stateMachine_->Update(deltaTime);
    }

    // 硬直・スタン・フェーズ移行スタン・テレポートのいずれかで aura を無効
    SetAuraEmitterActive(!isInRecovery_ && !IsStunned() && !isTeleporting_);

    hitFlashEffect_.Update(deltaTime, GetModel());

    shakeEffect_.Update(deltaTime);

    // Actor の位置は変えず、見た目だけを揺らす
    if (MeshComponent* mesh = GetOwner()->GetComponent<MeshComponent>()) {
        Transform offset = mesh->GetOffset();
        offset.translate = shakeEffect_.GetOffset();
        mesh->SetOffset(offset);
    }

    // 衝撃波の球はボスの位置に置き続ける
    if (Tako::Actor* sphere = GetOwner()->Resolve(repelShockwaveSphere_)) {
        sphere->GetTransform().translate = GetOwner()->GetTransform().translate;
    }
}

void Boss::SetRepelShockwaveSphereScale(float radius)
{
    if (Tako::Actor* sphere = GetOwner()->Resolve(repelShockwaveSphere_)) {
        sphere->GetTransform().scale = { radius, radius, radius };
    }
}

void Boss::OnHit(float damage, float shakeIntensityOverride)
{
    phaseManager_.ConsumePhaseChangeRequest();

    hp_ -= damage;
    hp_ = std::max<float>(hp_, 0.0f);

    // 閾値到達でフェーズ移行スタン発動。HP は閾値に固定
    if (phaseManager_.GetPhase() == 1 &&
        hp_ <= kPhaseTransitionStunThreshold &&
        !hasTriggeredPhaseTransitionStun_) {
        hp_ = kPhaseTransitionStunThreshold;
        TriggerPhaseTransitionStun();
    }

    float hitEffectDuration = Tako::GameSettings::Get<BossSettings>().hitEffectDuration;
    hitFlashEffect_.Start(Color::kWhite, hitEffectDuration);

    StartShake(shakeIntensityOverride);
}

void Boss::StartShake(float intensity)
{
    // Engine Settings で変えた値をすぐ試せるよう、揺らすたびに読み直す
    ApplyEffectSettings();
    shakeEffect_.Start(intensity);
}

#ifdef _DEBUG
bool Boss::DrawCustomImGui()
{

    ImGui::SeparatorText("Basic Status");

    ImGui::Text("HP: %.1f / %.1f (%.1f%%)", hp_, kMaxHp, (hp_ / kMaxHp) * 100.0f);
    ImGui::ProgressBar(hp_ / kMaxHp, ImVec2(-1.0f, 0.0f), "");

    ImGui::Text("Life: %d", phaseManager_.GetLife());
    ImGui::Text("Phase: %d", phaseManager_.GetPhase());

    if (phaseManager_.IsDead()) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Dead: YES");
    }
    else {
        ImGui::Text("Dead: NO");
    }
    ImGui::SameLine();
    if (isPause_) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Paused: YES");
    }
    else {
        ImGui::Text("Paused: NO");
    }

    if (stateMachine_) {
        ImGui::Text("State: %s", stateMachine_->GetCurrentStateName().c_str());
    }

    ImGui::Text("Ready to Change Phase: %s", phaseManager_.IsReadyToChangePhase() ? "YES" : "NO");

    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::Text("Position: (%.2f, %.2f, %.2f)",
            GetOwner()->GetTransform().translate.x, GetOwner()->GetTransform().translate.y, GetOwner()->GetTransform().translate.z);
        ImGui::Text("Rotation: (%.2f, %.2f, %.2f)",
            GetOwner()->GetTransform().rotate.x, GetOwner()->GetTransform().rotate.y, GetOwner()->GetTransform().rotate.z);
        ImGui::Text("Scale: (%.2f, %.2f, %.2f)",
            GetOwner()->GetTransform().scale.x, GetOwner()->GetTransform().scale.y, GetOwner()->GetTransform().scale.z);
    }

    if (ImGui::CollapsingHeader("Shake Effect")) {
        ImGui::Text("Is Shaking: %s", shakeEffect_.IsActive() ? "YES" : "NO");
        ImGui::Text("Timer: %.3f / %.3f", shakeEffect_.GetTimer(), shakeEffect_.GetDuration());
        Vector3 offset = shakeEffect_.GetOffset();
        ImGui::Text("Offset: (%.3f, %.3f, %.3f)", offset.x, offset.y, offset.z);

        ImGui::Separator();
        ImGui::TextDisabled("Duration / Intensity: Engine Settings > Game > Boss");
        if (ImGui::Button("Test Shake")) {
            StartShake();
        }
    }

    float tempHp = hp_;
    if (ImGui::SliderFloat("Set HP", &tempHp, 0.0f, kMaxHp)) {
        hp_ = std::clamp(tempHp, 0.0f, kMaxHp);
    }

    ImVec2 buttonSize(ImGui::GetContentRegionAvail().x * 0.48f, 0);
    if (ImGui::Button("Set Phase 1", buttonSize)) {
        SetPhase(1);
        hp_ = kMaxHp;
        phaseManager_.Reset();
    }
    ImGui::SameLine();
    if (ImGui::Button("Set Phase 2", buttonSize)) {
        SetPhase(2);
        hp_ = kPhase2InitialHp;
    }

    ImGui::Spacing();
    ImGui::Checkbox("Pause Boss", &isPause_);

    ImGui::Spacing();
    ImVec2 fullButtonSize(ImGui::GetContentRegionAvail().x * 0.48f, 0);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
    if (ImGui::Button("Kill Boss", fullButtonSize)) {
        phaseManager_.SetDead(true);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
    if (ImGui::Button("Revive Boss", fullButtonSize)) {
        phaseManager_.Reset();
        hp_ = kMaxHp;
    }
    ImGui::PopStyleColor(3);

    return false;
}
#endif

void Boss::RequestBulletSpawn(const Vector3& position, const Vector3& velocity) {
    Bullet::Spawn(*GetOwner()->GetScene(), "BossBullet", position, velocity);
}

void Boss::RequestPenetratingBulletSpawn(const Vector3& position, const Vector3& velocity) {
    Bullet::Spawn(*GetOwner()->GetScene(), "PenetratingBossBullet", position, velocity);
}

void Boss::SetPlayer(Player* player) {
    player_ = player;
    if (Tako::BehaviorTree* tree = GetBehaviorTree()) {
        tree->GetBlackboard()->SetPtr<Player>("player", player);
    }
}

void Boss::SetAttackSignEmitterActive(bool active) {
    if (emitterManager_) {
        emitterManager_->SetEmitterActive(attackSignEmitterName_, active);
    }
}

void Boss::SetAttackSignEmitterPosition(const Vector3& position) {
    if (emitterManager_) {
        emitterManager_->SetEmitterPosition(attackSignEmitterName_, position);
    }
}

void Boss::SetBulletSignEmitterActive(bool active) {
    if (emitterManager_) {
        emitterManager_->SetEmitterActive(bulletSignEmitterName_, active);
    }
}

void Boss::SetBulletSignEmitterPosition(const Vector3& position) {
    if (emitterManager_) {
        emitterManager_->SetEmitterPosition(bulletSignEmitterName_, position);
    }
}

void Boss::SetBulletSignEmitterScaleRangeX(float value) {
    if (emitterManager_) {
        emitterManager_->SetEmitterScaleRange(
            bulletSignEmitterName_,
            Vector2(value, value),
            Vector2(1.0f, 1.0f)
        );
    }
}

Vector4 Boss::GetBaseColor() const {
    const Tako::Object3d* model = GetModel();
    return model ? model->GetBaseMaterialColor() : Color::kWhite;
}

void Boss::StartStunFlash(const Vector4& color, float duration) {
    hitFlashEffect_.Start(color, duration);
}

void Boss::EnterRecovery() {
    isInRecovery_ = true;
}

void Boss::ExitRecovery() {
    isInRecovery_ = false;
}

void Boss::SetDashing(bool dashing) {
    isDashing_ = dashing;
}

bool Boss::IsStunned() const {
    if (!stateMachine_) return false;
    const std::string& state = stateMachine_->GetCurrentStateName();
    return state == "Stunned" || state == "PhaseTransitionStun";
}

bool Boss::IsInPhaseTransitionStun() const {
    if (!stateMachine_) return false;
    return stateMachine_->GetCurrentStateName() == "PhaseTransitionStun";
}

void Boss::ResetActionState() {
    isInRecovery_ = false;
    isDashing_ = false;
    SetMeleeAttackBlockVisible(false);
    SetAttackSignEmitterActive(false);
    SetBulletSignEmitterActive(false);
    if (meleeAttackCollider_) {
        meleeAttackCollider_->SetActive(false);
    }
}

void Boss::OnMeleeAttackHit(float damage, const Vector3& knockbackDir, bool isKnockbackCombo) {
    if (isDashing_) return;

    const std::string& currentState = stateMachine_->GetCurrentStateName();

    if (currentState == "Stunned") {
        OnHit(damage, 1.0f);
        if (isKnockbackCombo) {
            auto* stunnedState = static_cast<BossStunnedState*>(
                stateMachine_->GetState("Stunned"));
            stunnedState->EnableKnockback(knockbackDir);
        }
        CameraManager::GetInstance()->StartShake(0.3f);
        FrameTimer::GetInstance()->SetTimeScaleForDuration(0.0f, Tako::GameSettings::Get<GameplaySettings>().hitStopDuration);
        return;
    }

    if (currentState == "PhaseTransitionStun") {
        CompletePhaseTransition();
        CameraManager::GetInstance()->StartShake(0.3f);
        FrameTimer::GetInstance()->SetTimeScaleForDuration(0.0f, Tako::GameSettings::Get<GameplaySettings>().hitStopDuration);
        return;
    }

    if (currentState == "Normal") {
        // 硬直中 or 強制脆弱ならダメージ + スタン、それ以外は離脱
        if (isInRecovery_ || forceVulnerable_) {
            OnHit(damage, 1.0f);
            CameraManager::GetInstance()->StartShake(0.3f);
            FrameTimer::GetInstance()->SetTimeScaleForDuration(0.0f, Tako::GameSettings::Get<GameplaySettings>().hitStopDuration);
            pendingStunDirection_ = knockbackDir;
            pendingStunWithKnockback_ = isKnockbackCombo;
            stateMachine_->ChangeState("Stunned");
        }
        else {
            stateMachine_->ChangeState("Retreating");
        }
        return;
    }
}

void Boss::TriggerPhaseTransitionStun() {
    if (hasTriggeredPhaseTransitionStun_) {
        return;
    }
    hasTriggeredPhaseTransitionStun_ = true;

    stateMachine_->ChangeState("PhaseTransitionStun");
}

void Boss::CompletePhaseTransition() {
    hp_ = kPhase2InitialHp;
    phaseManager_.SetPhase(2);

    // Normal へ復帰（PhaseTransitionStunState::Exit() でパーティクル無効化）
    stateMachine_->ChangeState("Normal");
}

void Boss::SetCanAttackSignEmitterActive(bool active) {
    if (emitterManager_) {
        emitterManager_->SetEmitterActive(canAttackSignEmitterName_, active);
    }
}

void Boss::SetCanAttackSignEmitterPosition(const Vector3& position) {
    if (emitterManager_) {
        emitterManager_->SetEmitterPosition(canAttackSignEmitterName_, position);
    }
}