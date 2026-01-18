#include "Boss.h"

#include <algorithm>
#include "Object3d.h"
#include "OBBCollider.h"
#include "CollisionManager.h"
#include "../../Collision/CollisionTypeIdDef.h"
#include "../../Collision/BossMeleeAttackCollider.h"
#include "FrameTimer.h"
#include "WinApp.h"
#include "BossBehaviorTree/BossBehaviorTree.h"
#include "GlobalVariables.h"
#include "EmitterManager.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#include "BossNodeEditor/BossNodeEditor.h"
#endif

using namespace Tako;

Boss::Boss()
{
}

Boss::~Boss()
{
}

void Boss::Initialize()
{
    // GlobalVariablesから値を取得
    GlobalVariables* gv = GlobalVariables::GetInstance();

    model_ = std::make_unique<Object3d>();
    model_->Initialize();
    model_->SetModel("white_cube.gltf");
    model_->SetMaterialColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    transform_.translate = Vector3(0.0f, initialY_, initialZ_);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.scale = Vector3(1.0f, 1.0f, 1.0f);

    model_->SetTransform(transform_);

    // HPバーUIの初期化（2段バー：フェーズ1=青、フェーズ2=赤）
    hpBar_.InitializeDual(
        "white.png",
        Vector2(500.0f, 30.0f),
        0.65f,  // 画面X比率
        0.05f,  // 画面Y比率
        Vector4{ 0.5f, 0.5f, 1.0f, 1.0f },  // フェーズ1: 青
        Vector4{ 1.0f, 0.3f, 0.3f, 1.0f }   // フェーズ2: 赤
    );

    // フェーズマネージャーの初期化
    phaseManager_.Initialize(kMaxHp, kPhase2Threshold, kPhase2InitialHp);

    // Colliderの初期化
    float bodySize = gv->GetValueFloat("Boss", "BodyColliderSize");
    bodyCollider_ = std::make_unique<OBBCollider>();
    bodyCollider_->SetTransform(&transform_);
    bodyCollider_->SetSize(Vector3(bodySize, bodySize, bodySize));
    bodyCollider_->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
    bodyCollider_->SetTypeID(static_cast<uint32_t>(CollisionTypeId::BOSS));
    bodyCollider_->SetOwner(this);

    // CollisionManagerに登録
    CollisionManager::GetInstance()->AddCollider(bodyCollider_.get());

    // 近接攻撃用ブロックの初期化
    meleeAttackBlock_ = std::make_unique<Object3d>();
    meleeAttackBlock_->Initialize();
    meleeAttackBlock_->SetModel("white_cube.gltf");
    meleeAttackBlock_->SetMaterialColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));  // オレンジ色

    // 近接攻撃コライダーの初期化
    float meleeColliderSizeX = gv->GetValueFloat("BossMeleeAttackCollider", "ColliderSizeX");
    float meleeColliderSizeY = gv->GetValueFloat("BossMeleeAttackCollider", "ColliderSizeY");
    float meleeColliderSizeZ = gv->GetValueFloat("BossMeleeAttackCollider", "ColliderSizeZ");
    float meleeOffsetZ = gv->GetValueFloat("BossMeleeAttackCollider", "OffsetZ");
    meleeAttackCollider_ = std::make_unique<BossMeleeAttackCollider>(this);
    meleeAttackCollider_->SetTransform(&transform_);
    meleeAttackCollider_->SetSize(Vector3(meleeColliderSizeX, meleeColliderSizeY, meleeColliderSizeZ));
    meleeAttackCollider_->SetOffset(Vector3(0.0f, 0.0f, meleeOffsetZ));
    meleeAttackCollider_->SetOwner(this);


    // 近接攻撃コライダーをCollisionManagerに登録
    CollisionManager::GetInstance()->AddCollider(meleeAttackCollider_.get());

    // シェイクエフェクトパラメータの読み込み
    shakeEffect_.SetDefaultDuration(gv->GetValueFloat("Boss", "ShakeDuration"));
    shakeEffect_.SetDefaultIntensity(gv->GetValueFloat("Boss", "ShakeIntensity"));

    // ビヘイビアツリーの初期化
    behaviorTree_ = std::make_unique<BossBehaviorTree>(this, player_);

#ifdef _DEBUG
    // ノードエディタの初期化
    nodeEditor_ = std::make_unique<BossNodeEditor>();
    nodeEditor_->Initialize();
    BTNodePtr runtimeTree = nodeEditor_->BuildRuntimeTree();
    if (runtimeTree && behaviorTree_) {
        behaviorTree_->SetRootNode(runtimeTree);
    }
#endif

}

void Boss::Finalize()
{
    // Colliderを削除
    if (bodyCollider_) {
        CollisionManager::GetInstance()->RemoveCollider(bodyCollider_.get());
    }
    if (meleeAttackCollider_) {
        CollisionManager::GetInstance()->RemoveCollider(meleeAttackCollider_.get());
    }
}

void Boss::Update(float deltaTime)
{
    // HPバーの更新（2段バー）
    hpBar_.UpdateDual(hp_, kMaxHp, kPhase2Threshold, phaseManager_.GetPhase());

    // フェーズとライフの更新
    phaseManager_.Update(hp_);

    // AIシステムの更新
    if (!phaseManager_.IsDead() && !isPause_) {
        if (behaviorTree_) {
            // ビヘイビアツリーの更新
            behaviorTree_->Update(deltaTime);

#ifdef _DEBUG
            // エディタが有効な場合、実行中のノードをハイライト
            if (nodeEditor_ && showNodeEditor_) {
                BTNodePtr currentNode = behaviorTree_->GetCurrentRunningNode();
                if (currentNode) {
                    nodeEditor_->HighlightRunningNode(currentNode);
                }
            }
#endif
        }
    }

    // ヒットエフェクトの更新
    static const Vector4 kOriginalColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);  // 赤（元の色）
    hitFlashEffect_.Update(deltaTime, model_.get(), kOriginalColor);

    // シェイクエフェクトの更新
    shakeEffect_.Update(deltaTime);

    // モデルの更新（シェイクオフセットを適用）
    Transform renderTransform = transform_;
    renderTransform.translate += shakeEffect_.GetOffset();
    model_->SetTransform(renderTransform);
    model_->Update();
}

void Boss::Draw()
{
    model_->Draw();

    // 攻撃ブロックの描画（表示フラグがtrueの時のみ）
    if (meleeAttackBlockVisible_ && meleeAttackBlock_) {
        meleeAttackBlock_->Draw();
    }
}

void Boss::DrawSprite()
{
    hpBar_.Draw();
}

void Boss::OnHit(float damage, float shakeIntensityOverride)
{
    // フェーズ変更処理（準備完了なら変更）
    phaseManager_.ConsumePhaseChangeRequest();

    hp_ -= damage;
    hp_ = std::max<float>(hp_, 0.0f);

    // ヒットフラッシュエフェクト開始（白く光る）
    float hitEffectDuration = GlobalVariables::GetInstance()->GetValueFloat("Boss", "HitEffectDuration");
    hitFlashEffect_.Start(Vector4(1.0f, 1.0f, 1.0f, 1.0f), hitEffectDuration);

    // シェイクエフェクト開始
    StartShake(shakeIntensityOverride);
}

void Boss::StartShake(float intensity)
{
    shakeEffect_.Start(intensity);
}

void Boss::DrawImGui()
{
#ifdef _DEBUG

    // ===== 基本ステータス =====
    ImGui::SeparatorText("Basic Status");

    // HP表示（数値 + プログレスバー）
    ImGui::Text("HP: %.1f / %.1f (%.1f%%)", hp_, kMaxHp, (hp_ / kMaxHp) * 100.0f);
    ImGui::ProgressBar(hp_ / kMaxHp, ImVec2(-1.0f, 0.0f), "");

    // ライフ、フェーズ
    ImGui::Text("Life: %d", phaseManager_.GetLife());
    ImGui::Text("Phase: %d", phaseManager_.GetPhase());

    // 状態フラグ（警告は赤色でハイライト）
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

    ImGui::Text("Ready to Change Phase: %s", phaseManager_.IsReadyToChangePhase() ? "YES" : "NO");

    // ===== 座標情報（折りたたみ可能） =====
    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::Text("Position: (%.2f, %.2f, %.2f)",
            transform_.translate.x, transform_.translate.y, transform_.translate.z);
        ImGui::Text("Rotation: (%.2f, %.2f, %.2f)",
            transform_.rotate.x, transform_.rotate.y, transform_.rotate.z);
        ImGui::Text("Scale: (%.2f, %.2f, %.2f)",
            transform_.scale.x, transform_.scale.y, transform_.scale.z);

        // 初期位置の調整
        ImGui::Separator();
        ImGui::Text("Initial Position (for respawn):");
        ImGui::DragFloat("Initial Y", &initialY_, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat("Initial Z", &initialZ_, 1.0f, -50.0f, 50.0f);
    }

    // ===== シェイクエフェクト（折りたたみ可能） =====
    if (ImGui::CollapsingHeader("Shake Effect")) {
        ImGui::Text("Is Shaking: %s", shakeEffect_.IsActive() ? "YES" : "NO");
        ImGui::Text("Timer: %.3f / %.3f", shakeEffect_.GetTimer(), shakeEffect_.GetDuration());
        Vector3 offset = shakeEffect_.GetOffset();
        ImGui::Text("Offset: (%.3f, %.3f, %.3f)", offset.x, offset.y, offset.z);

        ImGui::Separator();
        float duration = shakeEffect_.GetDefaultDuration();
        if (ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f, 2.0f)) {
            shakeEffect_.SetDefaultDuration(duration);
            GlobalVariables::GetInstance()->SetValue("Boss", "ShakeDuration", duration);
        }
        float intensity = shakeEffect_.GetDefaultIntensity();
        if (ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 1.0f)) {
            shakeEffect_.SetDefaultIntensity(intensity);
            GlobalVariables::GetInstance()->SetValue("Boss", "ShakeIntensity", intensity);
        }

        if (ImGui::Button("Test Shake")) {
            StartShake();
        }
    }

    // ===== セクション6: コライダー（折りたたみ可能） =====
    if (ImGui::CollapsingHeader("Collider")) {
        if (bodyCollider_) {
            ImGui::Text("Active: %s", bodyCollider_->IsActive() ? "Yes" : "No");
            ImGui::Text("Type ID: %d (Enemy)", bodyCollider_->GetTypeID());

            Vector3 offset = bodyCollider_->GetOffset();
            ImGui::Text("Offset: (%.2f, %.2f, %.2f)", offset.x, offset.y, offset.z);

            Vector3 size = bodyCollider_->GetSize();
            ImGui::Text("Size: (%.2f, %.2f, %.2f)", size.x, size.y, size.z);

            Vector3 center = bodyCollider_->GetCenter();
            ImGui::Text("Center: (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
        }
    }

    // ===== ビヘイビアツリー =====
    ImGui::SeparatorText("Behavior Tree");

    // ビヘイビアツリーの制御
    if (behaviorTree_) {
        // JSONから直接ビヘイビアツリーに読み込み（デバッグ・リリース共通）
        ImGui::SameLine();
        if (ImGui::Button("Load Tree from JSON")) {
            if (behaviorTree_->LoadFromJSON("resources/Json/BossTree.json")) {
                ImGui::Text("Tree loaded successfully!");
            }
        }

        // デバッグビルド専用：ノードエディタ機能
        if (nodeEditor_) {
            ImGui::SameLine();
            if (ImGui::Button("Node Editor")) {
                showNodeEditor_ = !showNodeEditor_;
                nodeEditor_->SetVisible(showNodeEditor_);
            }

            // エディタのツリーをBehaviorTreeに適用
            ImGui::SameLine();
            if (ImGui::Button("Apply Editor Tree")) {
                BTNodePtr runtimeTree = nodeEditor_->BuildRuntimeTree();
                if (runtimeTree && behaviorTree_) {
                    behaviorTree_->SetRootNode(runtimeTree);
                }
            }
        }
    }

    // HP操作
    float tempHp = hp_;
    if (ImGui::SliderFloat("Set HP", &tempHp, 0.0f, kMaxHp)) {
        hp_ = std::clamp(tempHp, 0.0f, kMaxHp);
    }

    // フェーズ切り替え
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

    // 一時停止トグル
    ImGui::Spacing();
    ImGui::Checkbox("Pause Boss", &isPause_);

    // 死亡/復活コントロール
    ImGui::Spacing();
    ImVec2 fullButtonSize(ImGui::GetContentRegionAvail().x * 0.48f, 0);

    // Killボタン（赤色）
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
    if (ImGui::Button("Kill Boss", fullButtonSize)) {
        phaseManager_.SetDead(true);
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();

    // Reviveボタン（緑色）
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
    if (ImGui::Button("Revive Boss", fullButtonSize)) {
        phaseManager_.Reset();
        hp_ = kMaxHp;
    }
    ImGui::PopStyleColor(3);

    // ノードエディタの更新
    if (nodeEditor_ && showNodeEditor_) {
        nodeEditor_->Update();
    }

#endif
}

void Boss::RequestBulletSpawn(const Vector3& position, const Vector3& velocity) {
    bulletSpawner_.RequestSpawn(position, velocity);
}

std::vector<BulletSpawnRequest> Boss::ConsumePendingBullets() {
    return bulletSpawner_.Consume();
}

void Boss::SetPlayer(Player* player) {
    player_ = player;
    if (behaviorTree_) {
        behaviorTree_->SetPlayer(player);
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