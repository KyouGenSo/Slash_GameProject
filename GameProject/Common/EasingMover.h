#pragma once
#include "Vector3.h"

/// <summary>
/// イージング移動システムクラス
/// 開始位置から目標位置へスムーズに移動する機能を提供
/// </summary>
class EasingMover
{
public:
    /// <summary>
    /// イージングタイプ
    /// </summary>
    enum class EasingType {
        Linear,      ///< 線形補間
        SmoothStep,  ///< スムーズステップ（デフォルト）
        EaseOut      ///< イーズアウト
    };

    EasingMover() = default;
    ~EasingMover() = default;

    /// <summary>
    /// 移動を初期化
    /// </summary>
    /// <param name="start">開始位置</param>
    /// <param name="target">目標位置</param>
    /// <param name="duration">移動にかかる時間（秒）</param>
    void Initialize(const Tako::Vector3& start, const Tako::Vector3& target, float duration);

    /// <summary>
    /// 距離と速度から移動を初期化
    /// </summary>
    /// <param name="start">開始位置</param>
    /// <param name="target">目標位置</param>
    /// <param name="speed">移動速度（単位/秒）</param>
    void InitializeWithSpeed(const Tako::Vector3& start, const Tako::Vector3& target, float speed);

    /// <summary>
    /// 移動を更新し、現在位置を返す
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間</param>
    /// <returns>現在の補間位置</returns>
    Tako::Vector3 Update(float deltaTime);

    /// <summary>
    /// 移動状態をリセット
    /// </summary>
    void Reset();

    /// <summary>
    /// 目標位置に到達したか判定
    /// </summary>
    /// <returns>true: 到達済み, false: 移動中</returns>
    bool HasReached() const;

    /// <summary>
    /// 移動初期化済みか
    /// </summary>
    /// <returns>true: 初期化済み, false: 未初期化</returns>
    bool IsInitialized() const;

    /// <summary>
    /// 移動進捗を取得（0.0〜1.0）
    /// </summary>
    /// <returns>0.0: 開始, 1.0: 到達</returns>
    float GetProgress() const;

    /// <summary>
    /// イージングタイプを設定
    /// </summary>
    /// <param name="type">イージングタイプ</param>
    void SetEasingType(EasingType type);

    /// <summary>
    /// 目標位置を取得
    /// </summary>
    /// <returns>目標位置</returns>
    const Tako::Vector3& GetTargetPosition() const;

    /// <summary>
    /// 開始位置を取得
    /// </summary>
    /// <returns>開始位置</returns>
    const Tako::Vector3& GetStartPosition() const;

private:
    /// <summary>
    /// イージング関数を適用
    /// </summary>
    /// <param name="t">正規化された時間（0.0〜1.0）</param>
    /// <returns>イージング適用後の値</returns>
    float ApplyEasing(float t) const;

private:

    Tako::Vector3 startPosition_;   ///< 開始位置
    Tako::Vector3 targetPosition_;  ///< 目標位置
    float elapsedTime_ = 0.0f;      ///< 経過時間
    float duration_ = 0.0f;         ///< 移動所要時間
    bool isInitialized_ = false;    ///< 初期化済みフラグ
    EasingType easingType_ = EasingType::SmoothStep;  ///< イージングタイプ
};
