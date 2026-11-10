#pragma once
#include "ICameraController.h"
#include "../CameraSettings.h"
#include "Vector3.h"

/// <summary>
/// 俯瞰視点の固定角度カメラコントローラー
/// </summary>
class TopDownController : public TargetedCameraController {
public: //メンバー関数
    TopDownController();
    ~TopDownController() override = default;

    void Update(float deltaTime) override;
    void Activate() override;
    void Deactivate() override { isActive_ = false; }
    void Reset();

    //========================================================
    //Setter
    //========================================================
    /// <summary>
    /// 調整値をまとめて反映する（GameCamera の Inspector の値）
    /// </summary>
    /// <param name="resetsCurrent">true なら、今の高さと引きも始まりの値に戻す</param>
    void ApplySettings(const TopDownCameraSettings& settings, bool resetsCurrent);

    void SetBaseHeight(float height) {
        baseHeight_ = height;
    }

    /// <param name="angleX">ラジアン</param>
    void SetCameraAngle(float angleX) {
        cameraAngleX_ = angleX;
    }

    void SetHeightMultiplier(float multiplier) {
        heightMultiplier_ = multiplier;
    }

    /// <param name="smoothness">0.0-1.0</param>
    void SetSmoothness(float smoothness) {
        followSmoothness_ = smoothness;
    }

    //========================================================
    //Getter
    //========================================================
    bool IsActive() const override { return isActive_; }

    CameraControlPriority GetPriority() const override {
        return CameraControlPriority::FOLLOW_DEFAULT;
    }

    float GetCurrentHeight() const { return currentHeight_; }

    const Tako::Vector3& GetInterpolatedTargetPosition() const {
        return interpolatedTargetPos_;
    }

private: //非公開関数
    /// <summary>
    /// 複数ターゲットの中心点を算出
    /// </summary>
    /// <returns>全ターゲットの平均位置。primaryTarget_ が無ければゼロベクトル</returns>
    Tako::Vector3 CalculateFocusPoint() const;

    /// <summary>
    /// 全ターゲット間の最大距離を算出
    /// </summary>
    /// <returns>最大距離。追加ターゲットが無ければ 0</returns>
    float CalculateMaxTargetDistance() const;

    /// <summary>
    /// ターゲット間距離からカメラの高さと後退量を算出
    /// </summary>
    /// <param name="targetDistance">ターゲット間の最大距離</param>
    /// <param name="outHeight">算出した高さ（minHeight_〜maxHeight_ でクランプ）</param>
    /// <param name="outBackOffset">算出した後退量（負値、minBackOffset_〜maxBackOffset_ でクランプ）</param>
    void CalculateCameraParameters(float targetDistance,
        float& outHeight,
        float& outBackOffset) const;

    /// <summary>
    /// 注視点・高さ・後退量を smoothing の割合で目標へ寄せてカメラへ反映
    /// </summary>
    void UpdateCameraPosition(float smoothing);

private: //メンバー変数
    Tako::Vector3 interpolatedTargetPos_ = {};
    float         currentHeight_         = TopDownCameraSettings{}.initialHeight;
    float         currentBackOffset_     = TopDownCameraSettings{}.initialBackOffset;

    float baseHeight_       = TopDownCameraSettings{}.baseHeight;
    float heightMultiplier_ = TopDownCameraSettings{}.heightMultiplier;
    float minHeight_        = TopDownCameraSettings{}.minHeight;
    float maxHeight_        = TopDownCameraSettings{}.maxHeight;

    float cameraAngleX_ = TopDownCameraSettings{}.angleX;

    float baseBackOffset_       = TopDownCameraSettings{}.baseBackOffset;
    float backOffsetMultiplier_ = TopDownCameraSettings{}.backOffsetMultiplier;
    float minBackOffset_        = TopDownCameraSettings{}.minBackOffset;
    float maxBackOffset_        = TopDownCameraSettings{}.maxBackOffset;

    float followSmoothness_ = TopDownCameraSettings{}.followSmoothness;

    float standardFov_ = TopDownCameraSettings{}.fov;
};
