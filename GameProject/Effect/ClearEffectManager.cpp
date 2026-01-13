#include "ClearEffectManager.h"
#include "EmitterManager.h"
#include "Object/Boss/Boss.h"
#include <algorithm>

ClearEffectManager::ClearEffectManager(Tako::EmitterManager* emitterManager)
    : TransitionEffectBase(emitterManager)
{
}

void ClearEffectManager::Start()
{
    if (isPlaying_) {
        return;
    }

    timer_ = 0.0f;
    isPlaying_ = true;
    isComplete_ = false;
    phase_ = Phase::SlashBuildup;
    currentSlashCount_ = 1;
    currentSlashRadius_ = 2.0f;
    explosionFired_ = false;
}

void ClearEffectManager::Update(float deltaTime)
{
    if (!isPlaying_ || isComplete_) {
        return;
    }

    // タイマー更新
    timer_ += deltaTime;

    // ボス位置にエミッター位置を設定
    if (target_) {
        Tako::Vector3 bossPos = target_->GetTranslate();
        emitterManager_->SetEmitterPosition("clear_slash", bossPos);
    }

    // フェーズに応じた処理
    switch (phase_) {
    case Phase::SlashBuildup:
        // 開始遅延後に斬撃エフェクト開始
        if (timer_ > params_.startDelay) {
            emitterManager_->SetEmitterActive("clear_slash", true);
            emitterManager_->SetEmitterCount("clear_slash", currentSlashCount_);
            emitterManager_->SetEmitterRadius("clear_slash", currentSlashRadius_);

            // ボスシェイク
            if (target_) {
                target_->StartShake(params_.shakeDuration);
            }

            // 斬撃数と半径を増加
            if (currentSlashCount_ < params_.slashMaxCount || currentSlashRadius_ < params_.slashMaxRadius) {
                currentSlashCount_ += params_.slashCountIncrement;
                currentSlashRadius_ += params_.slashRadiusIncrement;
            }
            else {
                // 最大値到達で爆発フェーズに遷移
                emitterManager_->SetEmitterActive("clear_slash", false);
                phase_ = Phase::Explosion;
            }
        }
        break;

    case Phase::Explosion:
        // 爆発エフェクト発火
        if (!explosionFired_) {
            if (target_) {
                target_->StartShake(params_.shakeDuration);

                // ボス位置に爆発エミッターを配置
                Tako::Vector3 bossPos = target_->GetTranslate();
                emitterManager_->SetEmitterPosition("over2", bossPos);
            }
            emitterManager_->CreateTemporaryEmitterFrom("over2", "over2_temp", 0.1f);
            explosionFired_ = true;
            phase_ = Phase::ScaleDown;
        }
        break;

    case Phase::ScaleDown:
        // ボススケールの減少
        if (target_) {
            Tako::Vector3 newScale = target_->GetScale() -
                Tako::Vector3(params_.scaleDecreaseRate, params_.scaleDecreaseRate, params_.scaleDecreaseRate) * deltaTime;
            newScale.x = std::max<float>(newScale.x, 0.0f);
            newScale.y = std::max<float>(newScale.y, 0.0f);
            newScale.z = std::max<float>(newScale.z, 0.0f);
            target_->SetScale(newScale);

            // スケールが0になったら完了
            if (newScale.x <= 0.0f) {
                phase_ = Phase::Complete;
                isComplete_ = true;
                isPlaying_ = false;
            }
        }
        break;

    case Phase::Complete:
    case Phase::Idle:
    default:
        break;
    }
}

void ClearEffectManager::Reset()
{
    TransitionEffectBase::Reset();
    phase_ = Phase::Idle;
    currentSlashCount_ = 1;
    currentSlashRadius_ = 2.0f;
    explosionFired_ = false;
}
