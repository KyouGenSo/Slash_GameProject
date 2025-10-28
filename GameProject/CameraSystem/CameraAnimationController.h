#pragma once
#include "ICameraController.h"
#include "CameraAnimation/CameraAnimation.h"
#include <memory>
#include <string>

/// <summary>
/// カメラアニメーションコントローラー
/// アニメーション再生を優先度システムで管理
/// </summary>
class CameraAnimationController : public ICameraController {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CameraAnimationController();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CameraAnimationController() override = default;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    void Update(float deltaTime) override;

    /// <summary>
    /// アクティブ状態を判定
    /// アニメーション再生中のみアクティブ
    /// </summary>
    /// <returns>アクティブな場合true</returns>
    bool IsActive() const override;

    /// <summary>
    /// 優先度を取得
    /// アニメーションは最高優先度
    /// </summary>
    /// <returns>コントローラーの優先度</returns>
    CameraControlPriority GetPriority() const override {
        return CameraControlPriority::ANIMATION;
    }

    /// <summary>
    /// アクティブ化（アニメーション再生開始）
    /// </summary>
    void Activate() override;

    /// <summary>
    /// 非アクティブ化（アニメーション停止）
    /// </summary>
    void Deactivate() override;

    /// <summary>
    /// カメラを設定
    /// </summary>
    /// <param name="camera">制御対象のカメラ</param>
    void SetCamera(Camera* camera) override;

    //==================== アニメーション制御 ====================

    /// <summary>
    /// アニメーションを読み込み
    /// </summary>
    /// <param name="filepath">JSONファイルパス</param>
    /// <returns>読み込み成功した場合true</returns>
    bool LoadAnimation(const std::string& filepath);

    /// <summary>
    /// アニメーション再生
    /// </summary>
    void Play();

    /// <summary>
    /// アニメーション一時停止
    /// </summary>
    void Pause();

    /// <summary>
    /// アニメーション停止
    /// </summary>
    void Stop();

    /// <summary>
    /// アニメーションリセット
    /// </summary>
    void Reset();

    //==================== キーフレーム管理 ====================

    /// <summary>
    /// キーフレームを追加
    /// </summary>
    /// <param name="keyframe">追加するキーフレーム</param>
    void AddKeyframe(const CameraKeyframe& keyframe);

    /// <summary>
    /// 現在のカメラ状態からキーフレームを追加
    /// </summary>
    /// <param name="time">キーフレームの時刻</param>
    /// <param name="interpolation">補間タイプ</param>
    void AddKeyframeFromCurrentCamera(float time,
        CameraKeyframe::InterpolationType interpolation =
            CameraKeyframe::InterpolationType::LINEAR);

    /// <summary>
    /// キーフレームを削除
    /// </summary>
    /// <param name="index">削除するキーフレームのインデックス</param>
    void RemoveKeyframe(size_t index);

    /// <summary>
    /// すべてのキーフレームをクリア
    /// </summary>
    void ClearKeyframes();

    //==================== Setter ====================

    /// <summary>
    /// ループ設定
    /// </summary>
    /// <param name="loop">ループする場合true</param>
    void SetLooping(bool loop);

    /// <summary>
    /// 再生速度設定
    /// </summary>
    /// <param name="speed">再生速度（1.0が標準）</param>
    void SetPlaySpeed(float speed);

    /// <summary>
    /// アニメーション名設定
    /// </summary>
    /// <param name="name">アニメーション名</param>
    void SetAnimationName(const std::string& name);

    //==================== Getter ====================

    /// <summary>
    /// CameraAnimationオブジェクトを取得
    /// </summary>
    /// <returns>内部のアニメーションオブジェクト</returns>
    CameraAnimation* GetAnimation() { return animation_.get(); }

    /// <summary>
    /// 再生状態を取得
    /// </summary>
    /// <returns>現在の再生状態</returns>
    CameraAnimation::PlayState GetPlayState() const;

    /// <summary>
    /// アニメーションの総時間を取得
    /// </summary>
    /// <returns>総時間（秒）</returns>
    float GetDuration() const;

    /// <summary>
    /// 現在の再生時間を取得
    /// </summary>
    /// <returns>再生時間（秒）</returns>
    float GetCurrentTime() const;

    /// <summary>
    /// キーフレーム編集中かを判定
    /// </summary>
    /// <returns>編集中の場合true</returns>
    bool IsEditingKeyframe() const;

private:
    // カメラアニメーションオブジェクト
    std::unique_ptr<CameraAnimation> animation_;

    // 再生完了時に自動で非アクティブ化するかのフラグ
    bool autoDeactivateOnComplete_ = true;
};