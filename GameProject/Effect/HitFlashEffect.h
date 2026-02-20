#pragma once
#include "Vector4.h"

// 前方宣言
namespace Tako {
class Object3d;
}

/// <summary>
/// ヒットフラッシュエフェクトクラス
/// ダメージ時に一時的に色を変化させる演出を管理
/// </summary>
class HitFlashEffect
{
public:
    HitFlashEffect() = default;
    ~HitFlashEffect() = default;

    /// <summary>
    /// フラッシュエフェクトを開始
    /// </summary>
    /// <param name="flashColor">フラッシュ時の色</param>
    /// <param name="duration">持続時間（秒）</param>
    void Start(const Tako::Vector4& flashColor, float duration = 0.1f);

    /// <summary>
    /// フラッシュエフェクトを更新
    /// </summary>
    /// <param name="deltaTime">フレーム間隔（秒）</param>
    /// <param name="target">適用対象の Object3d</param>
    /// <param name="originalColor">元の色（フラッシュ終了後に戻す色）</param>
    void Update(float deltaTime, Tako::Object3d* target, const Tako::Vector4& originalColor);

    /// <summary>
    /// フラッシュがアクティブか
    /// </summary>
    /// <returns>アクティブなら true</returns>
    bool IsActive() const { return isActive_; }

    /// <summary>
    /// フラッシュを停止
    /// </summary>
    void Stop() { isActive_ = false; timer_ = 0.0f; }

    /// <summary>
    /// 現在のタイマーを取得（デバッグ用）
    /// </summary>
    float GetTimer() const { return timer_; }

private:
    bool isActive_ = false;           ///< フラッシュがアクティブか
    float timer_ = 0.0f;              ///< 経過時間
    float duration_ = 0.1f;           ///< 持続時間
    Tako::Vector4 flashColor_{};      ///< フラッシュ色
};
