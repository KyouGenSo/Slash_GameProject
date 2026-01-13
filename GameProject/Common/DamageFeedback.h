#pragma once
#include "Vector3.h"
#include <string>

// 前方宣言
namespace Tako {
class EmitterManager;
}

/// <summary>
/// ダメージフィードバッククラス
/// 被弾時・パリィ成功時のエフェクト（シェイク、振動、Vignette）を統合管理
/// </summary>
class DamageFeedback
{
public:
    /// <summary>
    /// 被弾フィードバックパラメータ
    /// </summary>
    struct HitParams {
        float shakeIntensity = 0.8f;         ///< カメラシェイク強度
        float vibrationLow = 0.2f;           ///< ゲームパッド振動（低周波）
        float vibrationHigh = 0.3f;          ///< ゲームパッド振動（高周波）
        float vibrationDuration = 0.25f;     ///< 振動持続時間
        float vignettePower = 0.4f;          ///< Vignette強度
        float vignetteRange = 45.0f;         ///< Vignetteの範囲
        Tako::Vector3 vignetteColor = { 1.0f, 0.0f, 0.0f };  ///< Vignette色（デフォルト赤）
        float vignetteDuration = 0.25f;      ///< Vignette持続時間
    };

    /// <summary>
    /// パリィ成功フィードバックパラメータ
    /// </summary>
    struct ParryParams {
        float shakeIntensity = 0.2f;         ///< カメラシェイク強度（軽め）
        float vibrationLow = 0.1f;           ///< ゲームパッド振動（低周波）
        float vibrationHigh = 0.15f;         ///< ゲームパッド振動（高周波）
        float vibrationDuration = 0.1f;      ///< 振動持続時間
        float vignettePower = 0.4f;          ///< Vignette強度
        float vignetteRange = 45.0f;         ///< Vignetteの範囲
        Tako::Vector3 vignetteColor = { 0.058f, 0.447f, 1.0f };  ///< Vignette色（青）
        float vignetteDuration = 0.3f;       ///< Vignette持続時間
        std::string emitterBaseName = "parry_success";  ///< エミッター名
        float emitterDuration = 0.5f;        ///< エミッター持続時間
    };

    /// <summary>
    /// 被弾フィードバックをトリガー
    /// カメラシェイク、ゲームパッド振動、Vignetteエフェクトを同時発生
    /// </summary>
    /// <param name="params">フィードバックパラメータ（デフォルト値で呼び出し可能）</param>
    static void TriggerHitFeedback(const HitParams& params = HitParams{});

    /// <summary>
    /// パリィ成功フィードバックをトリガー
    /// カメラシェイク、振動、Vignette、パーティクルを発生
    /// </summary>
    /// <param name="position">エフェクト発生位置</param>
    /// <param name="emitterManager">エミッターマネージャー（nullptrの場合パーティクル省略）</param>
    /// <param name="params">フィードバックパラメータ（デフォルト値で呼び出し可能）</param>
    static void TriggerParryFeedback(
        const Tako::Vector3& position,
        Tako::EmitterManager* emitterManager,
        const ParryParams& params = ParryParams{});

private:
    // インスタンス化禁止
    DamageFeedback() = delete;
    ~DamageFeedback() = delete;
};
