#include "BossBorderParticleManager.h"
#include "EmitterManager.h"

BossBorderParticleManager::BossBorderParticleManager(Tako::EmitterManager* emitterManager)
    : emitterManager_(emitterManager)
{
}

void BossBorderParticleManager::Update(int bossPhase, const Tako::Vector3& bossPosition)
{
    // ボスフェーズ2の境界線パーティクル制御
    bool shouldShowBorder = (bossPhase == 2);

    if (shouldShowBorder && !isActive_) {
        // フェーズ2突入時：境界線を有効化
        SetActive(true);
    }
    else if (!shouldShowBorder && isActive_) {
        // フェーズ1に戻った時：境界線を無効化
        SetActive(false);
    }

    // アクティブ中はボスの移動に追従
    if (isActive_) {
        UpdatePositions(bossPosition);
    }
}

void BossBorderParticleManager::SetActive(bool active)
{
    if (isActive_ == active) {
        return;
    }

    emitterManager_->SetEmitterActive("boss_border_left", active);
    emitterManager_->SetEmitterActive("boss_border_right", active);
    emitterManager_->SetEmitterActive("boss_border_front", active);
    emitterManager_->SetEmitterActive("boss_border_back", active);

    isActive_ = active;
}

void BossBorderParticleManager::UpdatePositions(const Tako::Vector3& bossPosition)
{
    // Y座標を0に固定してボス周囲に配置
    Tako::Vector3 basePos = Tako::Vector3(bossPosition.x, 0.0f, bossPosition.z);
    float areaSize = params_.areaSize;

    emitterManager_->SetEmitterPosition("boss_border_left",
        basePos + Tako::Vector3(0.0f, 0.0f, -areaSize));
    emitterManager_->SetEmitterPosition("boss_border_right",
        basePos + Tako::Vector3(0.0f, 0.0f, areaSize));
    emitterManager_->SetEmitterPosition("boss_border_front",
        basePos + Tako::Vector3(-areaSize, 0.0f, 0.0f));
    emitterManager_->SetEmitterPosition("boss_border_back",
        basePos + Tako::Vector3(areaSize, 0.0f, 0.0f));
}
