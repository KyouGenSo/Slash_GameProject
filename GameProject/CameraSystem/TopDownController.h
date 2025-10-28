#pragma once
#include "ICameraController.h"
#include "CameraConfig.h"
#include "Vector3.h"

/// <summary>
/// トップダウン視点カメラコントローラー
/// 俯瞰視点での固定角度カメラ制御
/// </summary>
class TopDownController : public TargetedCameraController {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    TopDownController();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~TopDownController() override = default;

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
    /// 基準高さを設定
    /// </summary>
    /// <param name="height">カメラ高さ</param>
    void SetBaseHeight(float height) {
        baseHeight_ = height;
    }

    /// <summary>
    /// カメラ角度を設定
    /// </summary>
    /// <param name="angleX">X軸角度（ラジアン）</param>
    void SetCameraAngle(float angleX) {
        cameraAngleX_ = angleX;
    }

    /// <summary>
    /// 高さ倍率を設定
    /// </summary>
    /// <param name="multiplier">距離に応じた高さ倍率</param>
    void SetHeightMultiplier(float multiplier) {
        heightMultiplier_ = multiplier;
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
    /// 現在のカメラ高さを取得
    /// </summary>
    /// <returns>カメラ高さ</returns>
    float GetCurrentHeight() const { return currentHeight_; }

    /// <summary>
    /// 補間されたターゲット位置を取得
    /// </summary>
    /// <returns>補間位置</returns>
    const Vector3& GetInterpolatedTargetPosition() const {
        return interpolatedTargetPos_;
    }

private:
    /// <summary>
    /// フォーカスポイントを計算
    /// 複数ターゲットの中心点を算出
    /// </summary>
    /// <returns>フォーカスポイント</returns>
    Vector3 CalculateFocusPoint() const;

    /// <summary>
    /// ターゲット間の最大距離を計算
    /// </summary>
    /// <returns>最大距離</returns>
    float CalculateMaxTargetDistance() const;

    /// <summary>
    /// カメラ高さと後方オフセットを計算
    /// </summary>
    /// <param name="targetDistance">ターゲット間距離</param>
    /// <param name="outHeight">計算された高さ（出力）</param>
    /// <param name="outBackOffset">計算された後方オフセット（出力）</param>
    void CalculateCameraParameters(float targetDistance,
                                  float& outHeight,
                                  float& outBackOffset) const;

    /// <summary>
    /// カメラ位置を更新
    /// </summary>
    void UpdateCameraPosition();

private:
    // 位置関連
    Vector3 interpolatedTargetPos_ = {};
    float currentHeight_ = CameraConfig::TopDown::BASE_HEIGHT;
    float currentBackOffset_ = CameraConfig::TopDown::BASE_BACK_OFFSET;

    // 設定パラメータ
    float baseHeight_ = CameraConfig::TopDown::BASE_HEIGHT;
    float heightMultiplier_ = CameraConfig::TopDown::HEIGHT_MULTIPLIER;
    float minHeight_ = CameraConfig::TopDown::MIN_HEIGHT;
    float maxHeight_ = CameraConfig::TopDown::MAX_HEIGHT;

    float cameraAngleX_ = CameraConfig::TopDown::DEFAULT_ANGLE_X;

    float baseBackOffset_ = CameraConfig::TopDown::BASE_BACK_OFFSET;
    float backOffsetMultiplier_ = CameraConfig::TopDown::BACK_OFFSET_MULTIPLIER;
    float minBackOffset_ = CameraConfig::TopDown::MIN_BACK_OFFSET;
    float maxBackOffset_ = CameraConfig::TopDown::MAX_BACK_OFFSET;

    float followSmoothness_ = CameraConfig::FOLLOW_SMOOTHNESS;
};