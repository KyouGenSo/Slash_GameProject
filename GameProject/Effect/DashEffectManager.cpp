#include "DashEffectManager.h"
#include "EmitterManager.h"
#include "Settings/GameplaySettings.h"
#include "Vec3Func.h"
#include <cmath>

DashEffectManager::DashEffectManager(Tako::EmitterManager* emitterManager)
    : emitterManager_(emitterManager)
{
}

void DashEffectManager::Update(float deltaTime, const Tako::Vector3& playerPosition, bool isDashing)
{
    if (isDashing && !previousIsDashing_) {
        emitterManager_->SetEmitterActive(params_.emitterName, true);
        isActive_ = true;
        emitterPosition_ = playerPosition;
    }

    // ダッシュ終了後も追いつくまで補間を継続する
    if (isActive_) {
        // GameplaySettings の値が 0 より大きければ params より優先
        float lerpSpeed = Tako::GameSettings::Get<GameplaySettings>().dashEffectLerpSpeed;
        if (lerpSpeed <= 0.0f) {
            lerpSpeed = params_.lerpSpeed;
        }

        // 指数減衰 t = 1 - e^(-speed * dt)。FPS非依存で同じ見た目になる
        float t = 1.0f - std::exp(-lerpSpeed * deltaTime);

        emitterPosition_ = Tako::Vec3::Lerp(emitterPosition_, playerPosition, t);

        emitterManager_->SetEmitterPosition(params_.emitterName, emitterPosition_);

        if (!isDashing) {
            float distanceSquared = playerPosition.DistanceSquared(emitterPosition_);

            if (distanceSquared < params_.stopThreshold * params_.stopThreshold) {
                emitterManager_->SetEmitterActive(params_.emitterName, false);
                isActive_ = false;
            }
        }
    }

    previousIsDashing_ = isDashing;
}

void DashEffectManager::InitializePosition(const Tako::Vector3& position)
{
    emitterPosition_ = position;
}
