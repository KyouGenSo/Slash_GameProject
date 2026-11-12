#pragma once
#include <numbers>

#include "ICameraController.h"
#include "Source/CameraSystem/CameraSettings.h"
#include "Vector3.h"
#include "Input.h"

/// <summary>
/// プレイヤー後方から追従し、ボス注視機能を持つ三人称カメラ
/// </summary>
class ThirdPersonController : public TargetedCameraController {
public: //メンバー関数
    ThirdPersonController();
    ~ThirdPersonController() override = default;

    void Update(float deltaTime) override;
    void Activate() override;
    void Deactivate() override { isActive_ = false; }
    void Reset();
    void EnableLookAtTarget(bool enable) {
        enableLookAtTarget_ = enable;
    }

    //========================================================
    //Setter
    //========================================================
    /// <summary>
    /// 調整値をまとめて反映する（GameCamera の Inspector の値）
    /// </summary>
    void ApplySettings(const ThirdPersonCameraSettings& settings);

    void SetOffset(const Tako::Vector3& offset) {
        offset_ = offset;
        offsetOrigin_ = offset;
    }

    void SetRotateSpeed(float speed) {
        rotateSpeed_ = speed;
    }

    /// <param name="smoothness">0.0-1.0</param>
    void SetSmoothness(float smoothness) {
        followSmoothness_ = smoothness;
    }

    /// <summary>
    /// 注視対象（ボスなど）を設定
    /// </summary>
    void SetSecondaryTarget(const Tako::Transform* target) {
        secondaryTarget_ = target;
    }

    //========================================================
    //Getter
    //========================================================
    bool IsActive() const override { return isActive_; }

    CameraControlPriority GetPriority() const override {
        return CameraControlPriority::FOLLOW_DEFAULT;
    }

    const Tako::Vector3& GetOffset() const { return offset_; }

    const Tako::Vector3& GetInterpolatedTargetPosition() const {
        return interpolatedTargetPos_;
    }

private: //非公開関数
    void ProcessInput(float deltaTime);
    void UpdateRotation(float deltaTime);
    void UpdatePosition(float deltaTime);

    /// <summary>
    /// カメラ回転を考慮したオフセットを計算
    /// </summary>
    /// <returns>回転適用後のワールド空間オフセット</returns>
    Tako::Vector3 CalculateOffset() const;

    /// <summary>
    /// セカンダリターゲットへの注視回転を計算
    /// </summary>
    /// <returns>注視に必要な回転角（ラジアン、見下ろし角を加算）。ターゲット未設定なら現在のカメラ回転</returns>
    Tako::Vector3 CalculateLookAtRotation() const;

private: //メンバー変数
    Tako::Input* input_ = nullptr;

    Tako::Vector3 interpolatedTargetPos_ = {};
    Tako::Vector3 offset_                = ThirdPersonCameraSettings{}.offset;
    Tako::Vector3 offsetOrigin_          = offset_;

    float defaultAngleX_           = ThirdPersonCameraSettings{}.angleX;                   ///< ラジアン
    float lookDownAngle_           = ThirdPersonCameraSettings{}.lookDownAngle;            ///< ラジアン
    float gamepadRotateMultiplier_ = ThirdPersonCameraSettings{}.gamepadRotateMultiplier;

    float destinationAngleX_ = defaultAngleX_;  ///< ラジアン
    float destinationAngleY_ = 0.0f;            ///< ラジアン
    float destinationAngleZ_ = 0.0f;            ///< ラジアン

    // 設定の回す速さ（度）をラジアンに変換
    float rotateSpeed_       = ThirdPersonCameraSettings{}.rotateSpeed * (std::numbers::pi_v<float> / 180.0f);
    float followSmoothness_  = ThirdPersonCameraSettings{}.followSmoothness;
    float offsetLerpSpeed_   = ThirdPersonCameraSettings{}.offsetLerpSpeed;
    float rotationLerpSpeed_ = ThirdPersonCameraSettings{}.rotationLerpSpeed;
    float standardFov_       = ThirdPersonCameraSettings{}.fov;

    bool isRotating_ = false;

    const Tako::Transform* secondaryTarget_    = nullptr;  ///< 注視対象（ボスなど）
    bool                   enableLookAtTarget_ = false;
};
