#pragma once
#include "Vector3.h"

/// <summary>
/// シェイクエフェクトクラス
/// オブジェクトに揺れエフェクトを適用するための汎用クラス
/// </summary>
class ShakeEffect
{
public:
    ShakeEffect() = default;
    ~ShakeEffect() = default;

    /// <summary>
    /// シェイクを開始
    /// </summary>
    /// <param name="intensity">シェイク強度（0以下でデフォルト値使用）</param>
    /// <param name="duration">持続時間（秒）</param>
    void Start(float intensity = 0.0f, float duration = 0.3f);

    /// <summary>
    /// シェイクを更新
    /// </summary>
    /// <param name="deltaTime">フレーム間隔（秒）</param>
    void Update(float deltaTime);

    /// <summary>
    /// シェイクを停止
    /// </summary>
    void Stop();

    /// <summary>
    /// シェイクオフセットを取得
    /// </summary>
    /// <returns>現在のシェイクオフセット</returns>
    Tako::Vector3 GetOffset() const { return offset_; }

    /// <summary>
    /// シェイクがアクティブか
    /// </summary>
    /// <returns>アクティブなら true</returns>
    bool IsActive() const { return isActive_; }

    /// <summary>
    /// デフォルト強度を設定
    /// </summary>
    /// <param name="intensity">デフォルト強度</param>
    void SetDefaultIntensity(float intensity) { defaultIntensity_ = intensity; }

    /// <summary>
    /// デフォルト持続時間を設定
    /// </summary>
    /// <param name="duration">デフォルト持続時間</param>
    void SetDefaultDuration(float duration) { defaultDuration_ = duration; }

    /// <summary>
    /// 現在のタイマーを取得（デバッグ用）
    /// </summary>
    float GetTimer() const { return timer_; }

    /// <summary>
    /// 現在の持続時間を取得（デバッグ用）
    /// </summary>
    float GetDuration() const { return duration_; }

    /// <summary>
    /// デフォルト強度を取得
    /// </summary>
    float GetDefaultIntensity() const { return defaultIntensity_; }

    /// <summary>
    /// デフォルト持続時間を取得
    /// </summary>
    float GetDefaultDuration() const { return defaultDuration_; }

private:
    bool isActive_ = false;           ///< シェイクがアクティブか
    float timer_ = 0.0f;              ///< 経過時間
    float duration_ = 0.3f;           ///< 現在のシェイク持続時間
    float intensity_ = 0.2f;          ///< 現在のシェイク強度
    float defaultIntensity_ = 0.2f;   ///< デフォルトシェイク強度
    float defaultDuration_ = 0.3f;    ///< デフォルト持続時間
    Tako::Vector3 offset_{};          ///< シェイクオフセット
};
