#pragma once

#include "Input.h"
#include "Mat4x4Func.h"
#include "Vec3Func.h"
#include "Transform.h"
#include "Camera.h"

#include <math.h>
#include <memory>
#include <DirectXMath.h>

class CameraAnimation;

/// <summary>
/// トップダウンビューのカメラ設定構造体
/// 高さと角度、後方オフセットの設定を管理
/// </summary>
struct TopDownSettings {
  float baseHeight = 10.0f;                              ///< 基準となるカメラ高さ
  float heightMultiplier = 0.6f;                         ///< ズーム時の高さ倍率
  float minHeight = 26.0f;                               ///< カメラ最小高さ
  float maxHeight = 500.0f;                              ///< カメラ最大高さ
  float angleX = DirectX::XMConvertToRadians(25.0f);     ///< X軸回転角度（ラジアン）
  float baseBackOffset = -10.0f;                         ///< 基準となる後方オフセット
  float backOffsetMultiplier = 1.5f;                     ///< ズーム時の後方オフセット倍率
  float minBackOffset = -500.0f;                         ///< 最小後方オフセット値
  float maxBackOffset = -52.0f;                          ///< 最大後方オフセット値
};

/// <summary>
/// 追従カメラクラス
/// ターゲットを追従し、一人称視点とトップダウン視点を切り替え可能
/// </summary>
class FollowCamera {

public: // メンバ関数
  FollowCamera();
  ~FollowCamera();

  /// <summary>
  /// 初期化
  /// </summary>
  /// <param name="camera">制御対象のカメラ</param>
  void Initialize(Camera* camera);

  /// <summary>
  /// 終了処理
  /// </summary>
  void Finalize();

  /// <summary>
  /// 更新処理
  /// </summary>
  void Update();

  /// <summary>
  /// 一人称視点モードに切り替え
  /// </summary>
  void FirstPersonMode();

  /// <summary>
  /// トップダウン視点モードに切り替え
  /// </summary>
  void TopDownMode();

  /// <summary>
  /// カメラ位置をリセット
  /// </summary>
  void Reset();

  /// <summary>
  /// オフセット値をリセット
  /// </summary>
  void ResetOffset();

  /// <summary>
  /// ImGuiデバッグ情報の描画
  /// </summary>
  void DrawImGui();

  /// <summary>
  /// カメラオフセットを計算
  /// </summary>
  /// <returns>計算されたオフセットベクトル</returns>
  Vector3 CalculateOffset() const;

  /// <summary>
  /// 画面揺れ効果を開始
  /// </summary>
  /// <param name="power">揺れの強さ</param>
  void ShakeScreen(float power);

  /// <summary>
  /// ゲーム開始時のカメラアニメーション再生
  /// </summary>
  void PlayStartCameraAnimation();

  //-----------------------------Setters------------------------------//
  /// <summary>
  /// 追従ターゲットを設定
  /// </summary>
  /// <param name="target">追従対象のTransform</param>
  void SetTarget(const Transform* target);

  /// <summary>
  /// 第二ターゲットを設定
  /// </summary>
  /// <param name="target2">第二ターゲットのTransform</param>
  void SetTarget2(const Transform* target2);

  /// <summary>
  /// カメラオフセットを設定
  /// </summary>
  /// <param name="offset">新しいオフセット値（Z値は原点として保存）</param>
  void SetOffset(const Vector3& offset)
  {
    offset_.x = offset.x;
    offset_.y = offset.y;
    offsetOrigin_.z = offset.z;
  }

  /// <summary>
  /// 回転速度を設定
  /// </summary>
  /// <param name="speed">新しい回転速度</param>
  void SetRotateSpeed(const float speed) { rotateSpeed_ = speed; }

  /// <summary>
  /// カメラモードを設定
  /// </summary>
  /// <param name="mode">true: 一人称視点, false: トップダウン視点</param>
  void SetMode(bool mode) { mode_ = mode; }

  //-----------------------------Getters------------------------------//
  /// <summary>
  /// カメラを取得
  /// </summary>
  /// <returns>現在のカメラのポインタ</returns>
  Camera* GetCamera() { return camera_; }

  /// <summary>
  /// ビュープロジェクション行列を取得
  /// </summary>
  /// <returns>ビュープロジェクション行列の参照</returns>
  const Matrix4x4& GetViewProjection() const { return camera_->GetViewProjectionMatrix(); }

  /// <summary>
  /// オフセット値を取得
  /// </summary>
  /// <returns>現在のオフセットベクトルの参照</returns>
  const Vector3& GetOffset() const { return offset_; }

  /// <summary>
  /// 第二ターゲットを取得
  /// </summary>
  /// <returns>第二ターゲットのTransformポインタ</returns>
  const Transform* GetTarget2() const { return target2_; }

  /// <summary>
  /// カメラモードを取得
  /// </summary>
  /// <returns>true: 一人称視点, false: トップダウン視点</returns>
  bool GetMode() const { return mode_; }

private: // メンバ変数
  /// カメラインスタンスへのポインタ（制御対象のカメラ）
  Camera* camera_;

  /// カメラアニメーション管理オブジェクト（ゲーム開始時のカメラワーク等）
  std::unique_ptr<CameraAnimation> cameraAnimation_;

  /// 主要な追従対象のTransform（通常はプレイヤー）
  const Transform* target_ = nullptr;

  /// 第二ターゲットのTransform（複数対象を注視する場合に使用）
  const Transform* target2_ = nullptr;

  /// 入力管理システムへのポインタ
  Input* input_ = nullptr;

  /// カメラモードフラグ（true: 一人称視点, false: トップダウン視点）
  bool mode_ = false;

  /// 補間されたターゲット位置（複数ターゲットがある場合の中間点）
  Vector3 interTargetPos_;

  /// 現在のカメラオフセット値（カメラの相対位置）
  Vector3 offset_ = { 0.0f, 2.0f, -40.0f };

  /// 初期状態のオフセット値（リセット時に使用）
  Vector3 offsetOrigin_ = { 0.0f, 2.0f, -40.0f };

  /// 補間係数（0.0〜1.0の範囲でカメラの追従速度を制御）
  float t_ = 0.18f;

  /// 目標Y軸回転角度（ラジアン）
  float destinationAngleY_ = 0.0f;

  /// 目標X軸回転角度（ラジアン、上下方向の角度）
  float destinationAngleX_ = 8.0f;

  /// 目標Z軸回転角度（ラジアン、ロール回転）
  float destinationAngleZ_ = 0.0f;

  /// 回転中フラグ（カメラが回転アニメーション中かを示す）
  bool isRotating_ = false;

  /// 回転速度（1フレーム当たりの回転補間係数、0.0〜1.0）
  float rotateSpeed_ = 0.05f;

  /// トップダウンモード専用パラメータ（高さ、角度、オフセット設定）
  TopDownSettings topDownSettings_;

};