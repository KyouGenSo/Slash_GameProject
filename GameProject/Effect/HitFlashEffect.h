#pragma once
#include <optional>

#include "Vector4.h"

namespace Tako {
    class Object3d;
}

/// <summary>
/// ダメージ時に一時的に色を変える演出を管理
/// </summary>
class HitFlashEffect
{
public: //メンバー関数
    HitFlashEffect() = default;
    ~HitFlashEffect() = default;

    /// <summary>
    /// フラッシュ演出を開始し、タイマーをリセットする
    /// </summary>
    /// <param name="flashColor">点滅中に適用する色（RGBA）</param>
    /// <param name="duration">この色を保持する時間（秒）</param>
    void Start(const Tako::Vector4& flashColor, float duration = 0.1f);

    /// <summary>
    /// duration 内は flashColor をコードの色として上書きし、経過後に始める前のコードの色へ戻す（無かったなら上書きを外す）
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    /// <param name="target">色を設定する対象（nullptr なら何もしない）</param>
    void Update(float deltaTime, Tako::Object3d* target);

    void Stop() { isActive_ = false; hasPrevious_ = false; timer_ = 0.0f; }

    //====================
    //Getter
    //====================
    bool IsActive() const { return isActive_; }
    float GetTimer() const { return timer_; }

private: //メンバー変数
    bool                         isActive_      = false;
    bool                         hasPrevious_   = false;  ///< previousColor_ を読み取り済みか
    float                        timer_         = 0.0f;
    float                        duration_      = 0.1f;
    Tako::Vector4                flashColor_{};
    std::optional<Tako::Vector4> previousColor_;          ///< 始める前のコードの色（無ければ nullopt）
};
