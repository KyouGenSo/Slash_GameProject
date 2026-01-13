#include "OverEffectManager.h"
#include "EmitterManager.h"
#include "Object/Player/Player.h"
#include <algorithm>

OverEffectManager::OverEffectManager(Tako::EmitterManager* emitterManager)
    : TransitionEffectBase(emitterManager)
{
}

void OverEffectManager::Start()
{
    if (isPlaying_) {
        return;
    }

    timer_ = 0.0f;
    isPlaying_ = true;
    isComplete_ = false;
    phase_ = Phase::WaitEmit1;
    emit1Fired_ = false;
    emit2Fired_ = false;
}

void OverEffectManager::Update(float deltaTime)
{
    if (!isPlaying_ || isComplete_) {
        return;
    }

    // タイマー更新
    timer_ += deltaTime;

    // プレイヤー位置にエミッター位置を設定
    if (target_) {
        Tako::Vector3 playerPos = target_->GetTranslate();
        emitterManager_->SetEmitterPosition("over1", playerPos);
        emitterManager_->SetEmitterPosition("over2", playerPos);
    }

    // フェーズに応じた処理
    switch (phase_) {
    case Phase::WaitEmit1:
        // 第1エミッター発火
        if (timer_ > params_.emit1Time && !emit1Fired_) {
            emitterManager_->CreateTemporaryEmitterFrom("over1", "over1_temp", 0.5f);
            emit1Fired_ = true;
            phase_ = Phase::WaitEmit2;
        }
        break;

    case Phase::WaitEmit2:
        // 第2エミッター発火
        if (timer_ > params_.emit2Time && !emit2Fired_) {
            emitterManager_->CreateTemporaryEmitterFrom("over2", "over2_temp", 0.1f);
            emit2Fired_ = true;
            phase_ = Phase::ScaleDown;
        }
        break;

    case Phase::ScaleDown:
        // プレイヤースケールの減少
        if (target_) {
            Tako::Vector3 newScale = target_->GetScale() -
                Tako::Vector3(params_.scaleDecreaseRate, params_.scaleDecreaseRate, params_.scaleDecreaseRate) * deltaTime;
            newScale.x = std::max<float>(newScale.x, 0.0f);
            newScale.y = std::max<float>(newScale.y, 0.0f);
            newScale.z = std::max<float>(newScale.z, 0.0f);
            target_->SetScale(newScale);
        }

        // 演出総時間経過で完了
        if (timer_ > params_.totalTime) {
            phase_ = Phase::Complete;
            isComplete_ = true;
            isPlaying_ = false;
        }
        break;

    case Phase::Complete:
    case Phase::Idle:
    default:
        break;
    }
}

void OverEffectManager::Reset()
{
    TransitionEffectBase::Reset();
    phase_ = Phase::Idle;
    emit1Fired_ = false;
    emit2Fired_ = false;
}
