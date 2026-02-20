#include "DamageFeedback.h"
#include "../CameraSystem/CameraManager.h"
#include "Input.h"
#include "PostEffectManager.h"
#include "PostEffectStruct.h"
#include "EmitterManager.h"
#include "RandomEngine.h"
#include "CameraSystem/CameraManager.h"

using namespace Tako;

void DamageFeedback::TriggerHitFeedback(const HitParams& params)
{
    // カメラシェイク
    CameraManager::GetInstance()->StartShake(params.shakeIntensity);

    // ゲームパッド振動
    Input::GetInstance()->SetVibration(
        params.vibrationLow,
        params.vibrationHigh,
        params.vibrationDuration);

    // Vignette エフェクト
    VignetteParam vignetteParam{};
    vignetteParam.power = params.vignettePower;
    vignetteParam.range = params.vignetteRange;
    vignetteParam.color = params.vignetteColor;
    PostEffectManager::GetInstance()->ApplyTemporaryEffect(
        "Vignette",
        params.vignetteDuration,
        vignetteParam);
}

void DamageFeedback::TriggerParryFeedback(
    const Vector3& position,
    EmitterManager* emitterManager,
    const ParryParams& params)
{
    // パーティクルエフェクト
    if (emitterManager) {
        emitterManager->SetEmitterPosition(params.emitterBaseName, position);

        // ユニークなエミッター名を生成
        int uniqueId = RandomEngine::GetInstance()->GetInt(0, 999999);
        std::string tempEmitterName = params.emitterBaseName + "_temp_" + std::to_string(uniqueId);

        emitterManager->CreateTemporaryEmitterFrom(
            params.emitterBaseName,
            tempEmitterName,
            params.emitterDuration);
        emitterManager->SetEmitterActive(tempEmitterName, true);
    }

    // カメラシェイク（軽め）
    CameraManager::GetInstance()->StartShake(params.shakeIntensity);

    // コントローラー振動
    Input::GetInstance()->SetVibration(
        params.vibrationLow,
        params.vibrationHigh,
        params.vibrationDuration);

    // Vignette エフェクト（青）
    VignetteParam vignetteParam{};
    vignetteParam.power = params.vignettePower;
    vignetteParam.range = params.vignetteRange;
    vignetteParam.color = params.vignetteColor;
    PostEffectManager::GetInstance()->ApplyTemporaryEffect(
        "Vignette",
        params.vignetteDuration,
        vignetteParam);
}
