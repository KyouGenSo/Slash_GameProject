#pragma once
#include "ICameraController.h"
#include "CameraConfig.h"
#include "Vector3.h"
#include "Input.h"

/// <summary>
/// 一人称視点カメラコントローラー
/// ターゲット追従と回転制御に特化
/// </summary>
class FirstPersonController : public TargetedCameraController {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    FirstPersonController();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~FirstPersonController() override = default;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    void Update(float deltaTime) override;

    /// <summary>
    /// アクティブ状態を判定
    /// </summary>
    /// <returns>アクティブな場合true</returns>
    bool IsActive() const override { return isActive_; }

    /// <summary>
    /// 優先度を取得
    /// </summary>
    /// <returns>コントローラーの優先度</returns>
    CameraControlPriority GetPriority() const override {
        return CameraControlPriority::FOLLOW_DEFAULT;
    }

    /// <summary>
    /// アクティブ化
    /// </summary>
    void Activate() override;

    /// <summary>
    /// 非アクティブ化
    /// </summary>
    void Deactivate() override { isActive_ = false; }

    /// <summary>
    /// カメラ位置をリセット
    /// </summary>
    void Reset();

    //==================== Setter ====================

    /// <summary>
    /// オフセットを設定
    /// </summary>
    /// <param name="offset">カメラオフセット</param>
    void SetOffset(const Vector3& offset) {
        offset_ = offset;
        offsetOrigin_ = offset;
    }

    /// <summary>
    /// 回転速度を設定
    /// </summary>
    /// <param name="speed">回転速度</param>
    void SetRotateSpeed(float speed) {
        rotateSpeed_ = speed;
    }

    /// <summary>
    /// 追従の滑らかさを設定
    /// </summary>
    /// <param name="smoothness">滑らかさ（0.0-1.0）</param>
    void SetSmoothness(float smoothness) {
        followSmoothness_ = smoothness;
    }

    //==================== Getter ====================

    /// <summary>
    /// 現在のオフセットを取得
    /// </summary>
    /// <returns>オフセット値</returns>
    const Vector3& GetOffset() const { return offset_; }

    /// <summary>
    /// 補間されたターゲット位置を取得
    /// </summary>
    /// <returns>補間位置</returns>
    const Vector3& GetInterpolatedTargetPosition() const {
        return interpolatedTargetPos_;
    }

private:
    /// <summary>
    /// 入力処理
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void ProcessInput(float deltaTime);

    /// <summary>
    /// カメラ回転を更新
    /// </summary>
    void UpdateRotation();

    /// <summary>
    /// カメラ位置を更新
    /// </summary>
    void UpdatePosition();

    /// <summary>
    /// オフセットを計算（回転を考慮）
    /// </summary>
    /// <returns>計算されたオフセット</returns>
    Vector3 CalculateOffset() const;

private:
    // 入力システム
    Input* input_ = nullptr;

    // 位置関連
    Vector3 interpolatedTargetPos_ = {};
    Vector3 offset_ = {
        CameraConfig::FirstPerson::DEFAULT_OFFSET_X,
        CameraConfig::FirstPerson::DEFAULT_OFFSET_Y,
        CameraConfig::FirstPerson::DEFAULT_OFFSET_Z
    };
    Vector3 offsetOrigin_ = offset_;

    // 回転関連
    float destinationAngleX_ = CameraConfig::FirstPerson::DEFAULT_ANGLE_X;
    float destinationAngleY_ = 0.0f;
    float destinationAngleZ_ = 0.0f;

    // パラメータ
    float rotateSpeed_ = CameraConfig::FirstPerson::DEFAULT_ROTATE_SPEED;
    float followSmoothness_ = CameraConfig::FOLLOW_SMOOTHNESS;
    float offsetLerpSpeed_ = CameraConfig::OFFSET_LERP_SPEED;
    float rotationLerpSpeed_ = CameraConfig::ROTATION_LERP_SPEED;
    float standardFov_ = CameraConfig::STANDARD_FOV;

    // 状態
    bool isRotating_ = false;
};