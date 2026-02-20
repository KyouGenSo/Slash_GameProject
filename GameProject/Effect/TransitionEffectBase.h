#pragma once

namespace Tako {
class EmitterManager;
}

/// <summary>
/// トランジション演出の基底クラス
/// ゲームオーバー/クリアなどの演出に共通するインターフェースを提供
/// </summary>
class TransitionEffectBase
{
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit TransitionEffectBase(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// 仮想デストラクタ
    /// </summary>
    virtual ~TransitionEffectBase() = default;

    /// <summary>
    /// 演出開始
    /// </summary>
    virtual void Start() = 0;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">フレーム間隔（秒）</param>
    virtual void Update(float deltaTime) = 0;

    /// <summary>
    /// リセット（再利用のため）
    /// </summary>
    virtual void Reset();

    /// <summary>
    /// 演出完了判定
    /// </summary>
    /// <returns>演出が完了していれば true</returns>
    bool IsComplete() const { return isComplete_; }

    /// <summary>
    /// 演出中判定
    /// </summary>
    /// <returns>演出中であれば true</returns>
    bool IsPlaying() const { return isPlaying_; }

    /// <summary>
    /// 経過時間を取得（デバッグ用）
    /// </summary>
    /// <returns>経過時間（秒）</returns>
    float GetTimer() const { return timer_; }

protected:
    Tako::EmitterManager* emitterManager_ = nullptr;  ///< エミッターマネージャー
    float timer_ = 0.0f;                              ///< 経過時間
    bool isPlaying_ = false;                          ///< 演出中フラグ
    bool isComplete_ = false;                         ///< 演出完了フラグ
};
