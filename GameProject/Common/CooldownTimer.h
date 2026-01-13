#pragma once

/// <summary>
/// 汎用クールダウンタイマークラス
/// アクション（パリィ、ダッシュ等）のクールダウン管理を担当
/// </summary>
class CooldownTimer
{
public:
    CooldownTimer() = default;
    ~CooldownTimer() = default;

    /// <summary>
    /// クールダウンを開始
    /// </summary>
    /// <param name="duration">クールダウン時間（秒）</param>
    void Start(float duration);

    /// <summary>
    /// クールダウンを更新（毎フレーム呼び出し）
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間</param>
    void Update(float deltaTime);

    /// <summary>
    /// クールダウンが完了したか（アクション可能か）
    /// </summary>
    /// <returns>true: アクション可能, false: クールダウン中</returns>
    bool IsReady() const;

    /// <summary>
    /// クールダウン残り時間を取得
    /// </summary>
    /// <returns>残り時間（秒）</returns>
    float GetRemainingTime() const;

    /// <summary>
    /// クールダウン進捗を取得（0.0〜1.0）
    /// </summary>
    /// <returns>0.0: 開始直後, 1.0: 完了</returns>
    float GetProgress() const;

    /// <summary>
    /// クールダウンをリセット（即座にアクション可能に）
    /// </summary>
    void Reset();

private:
    float remainingTime_ = 0.0f;  ///< クールダウン残り時間
    float duration_ = 0.0f;       ///< クールダウン全体の時間
};
