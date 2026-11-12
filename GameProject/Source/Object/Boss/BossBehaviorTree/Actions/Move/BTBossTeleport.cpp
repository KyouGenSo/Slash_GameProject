#include "BTBossTeleport.h"
#include "Script/Object/Boss/Boss.h"
#include "Source/Object/Boss/Movement/BossAreaBounds.h"
#include "Script/Object/Player/Player.h"
#include "Object3d.h"
#include "RandomEngine.h"
#include <algorithm>

using namespace Tako;

BTBossTeleport::BTBossTeleport() {
    name_ = "BossTeleport";
}

void BTBossTeleport::OnInitialize(Tako::BTBlackboard* blackboard, Boss* boss) {
    teleportFired_ = false;

    // Boss::Update 内で aura エミッタが自動無効化される
    boss->SetTeleporting(true);

    auto* rng = Tako::RandomEngine::GetInstance();
    const auto stageBounds = BossMovement::CalcStageBounds();

    switch (mode_) {
    case TeleportMode::Fixed: {
        targetTeleportPosition_ = Vector3(
            targetPositionX_, targetPositionY_, targetPositionZ_);
        break;
    }
    case TeleportMode::RandomFromBoss: {
        Vector3 dir = rng->GetRandomDirectionXZ();
        float dist = rng->GetFloat(randomMinDistance_, randomMaxDistance_);
        Vector3 candidate = boss->GetTranslate() + dir * dist;
        targetTeleportPosition_ = BossMovement::ClampToBounds(candidate, stageBounds);
        break;
    }
    case TeleportMode::RandomFromPlayer: {
        // Blackboard からプレイヤーを取得。null ならボス中心にフォールバック。
        Player* player = blackboard ? blackboard->GetPtr<Player>("player") : nullptr;
        const Vector3 center = player ? player->GetTranslate() : boss->GetTranslate();
        Vector3 dir = rng->GetRandomDirectionXZ();
        float dist = rng->GetFloat(playerRandomMinDistance_, playerRandomMaxDistance_);
        Vector3 candidate = center + dir * dist;
        targetTeleportPosition_ = BossMovement::ClampToBounds(candidate, stageBounds);
        break;
    }
    }

    // 半透明モードへ切替 + 元のマテリアル状態をキャッシュ
    Object3d* model = boss->GetModel();
    if (model) {
        originalMaterialColor_ = boss->GetBaseColor();
        model->SetTransparent(true);
        UpdateModelAlpha(model, 1.0f);
    }
}

Tako::BTNodeStatus BTBossTeleport::OnExecute(Tako::BTBlackboard* /*blackboard*/, Boss* boss, float deltaTime) {
    elapsedTime_ += deltaTime;

    Object3d* model = boss->GetModel();

    const float fadeOutEnd = fadeOutDuration_;
    const float fadeInEnd  = fadeOutDuration_ + fadeInDuration_;

    if (elapsedTime_ < fadeOutEnd) {
        // Phase 0: フェードアウト (alpha 1 → 0)
        const float t = (fadeOutDuration_ > 0.0001f)
            ? std::clamp(elapsedTime_ / fadeOutDuration_, 0.0f, 1.0f)
            : 1.0f;
        const float alpha = 1.0f - t;
        if (model) UpdateModelAlpha(model, alpha);
        boss->SetBodyParticleEmitterActive(true);
    }
    else if (!teleportFired_) {
        // Phase 1: 瞬間移動
        if (model) UpdateModelAlpha(model, 0.0f);
        boss->SetBodyParticleEmitterActive(false);
        boss->SetTranslate(targetTeleportPosition_);
        teleportFired_ = true;
    }
    else if (elapsedTime_ < fadeInEnd) {
        // Phase 2: フェードイン (alpha 0 → 1)
        const float t = (fadeInDuration_ > 0.0001f)
            ? std::clamp((elapsedTime_ - fadeOutDuration_) / fadeInDuration_, 0.0f, 1.0f)
            : 1.0f;
        const float alpha = t;
        if (model) UpdateModelAlpha(model, alpha);
        boss->SetBodyParticleEmitterActive(true);
    }
    else {
        // Phase 3: 完了
        boss->SetBodyParticleEmitterActive(false);
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossTeleport::OnCleanup() {
    // マテリアル状態を復帰 (中断時の safety 含む)
    if (cachedBoss_) {
        Object3d* model = cachedBoss_->GetModel();
        if (model) {
            model->SetMaterialColor(std::nullopt);
            model->SetTransparent(std::nullopt);
        }
        cachedBoss_->SetBodyParticleEmitterActive(false);
        // 解除後、次フレームの Boss::Update で aura が再有効化される
        cachedBoss_->SetTeleporting(false);
    }
    teleportFired_ = false;
}

void BTBossTeleport::UpdateModelAlpha(Tako::Object3d* model, float alpha) const {
    Vector4 c = originalMaterialColor_;
    c.w = alpha;
    model->SetMaterialColor(c);
}

void BTBossTeleport::PostEditChange() {
    randomMaxDistance_       = (std::max)(randomMaxDistance_, randomMinDistance_);
    playerRandomMaxDistance_ = (std::max)(playerRandomMaxDistance_, playerRandomMinDistance_);
}

void BTBossTeleport::PostLoad(const nlohmann::json& raw) {
    // 旧 bool キーから変換する（true → RandomFromBoss, false → Fixed）
    if (raw.is_object() && !raw.contains("mode") && raw.contains("useRandomPosition") && raw["useRandomPosition"].is_boolean()) {
        mode_ = raw["useRandomPosition"].get<bool>() ? TeleportMode::RandomFromBoss : TeleportMode::Fixed;
    }
}
