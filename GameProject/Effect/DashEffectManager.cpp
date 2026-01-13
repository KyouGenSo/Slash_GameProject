#include "DashEffectManager.h"
#include "EmitterManager.h"
#include "GlobalVariables.h"
#include "Vec3Func.h"
#include <cmath>

DashEffectManager::DashEffectManager(Tako::EmitterManager* emitterManager)
    : emitterManager_(emitterManager)
{
}

void DashEffectManager::Update(float deltaTime, const Tako::Vector3& playerPosition, bool isDashing)
{
    // ダッシュ開始時: エミッター有効化 & 位置リセット
    if (isDashing && !previousIsDashing_) {
        emitterManager_->SetEmitterActive(params_.emitterName, true);
        isActive_ = true;
        emitterPosition_ = playerPosition;
    }

    // エミッターがアクティブな間は補間を継続（ダッシュ終了後も追いつくまで続ける）
    if (isActive_) {
        // GlobalVariablesから補間速度を取得（設定されていればそちらを優先）
        float lerpSpeed = Tako::GlobalVariables::GetInstance()->GetValueFloat("DashEffect", "LerpSpeed");
        if (lerpSpeed <= 0.0f) {
            lerpSpeed = params_.lerpSpeed;
        }

        // フレームレート非依存の指数減衰補間
        // t = 1 - e^(-speed * dt) で、どのFPSでも同じ視覚的結果
        float t = 1.0f - std::exp(-lerpSpeed * deltaTime);

        // エミッター位置をプレイヤー位置に向かって補間
        emitterPosition_ = Tako::Vec3::Lerp(emitterPosition_, playerPosition, t);

        // エミッター位置を更新
        emitterManager_->SetEmitterPosition(params_.emitterName, emitterPosition_);

        // ダッシュ終了後、エミッターがプレイヤー位置に十分近づいたら無効化
        if (!isDashing) {
            Tako::Vector3 diff = playerPosition - emitterPosition_;
            float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

            if (distanceSquared < params_.stopThreshold * params_.stopThreshold) {
                emitterManager_->SetEmitterActive(params_.emitterName, false);
                isActive_ = false;
            }
        }
    }

    // 状態を保存
    previousIsDashing_ = isDashing;
}

void DashEffectManager::InitializePosition(const Tako::Vector3& position)
{
    emitterPosition_ = position;
}
