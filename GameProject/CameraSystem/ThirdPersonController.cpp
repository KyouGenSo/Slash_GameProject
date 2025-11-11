#include "ThirdPersonController.h"
#include "Vec3Func.h"
#include "Mat4x4Func.h"

ThirdPersonController::ThirdPersonController() {
  input_ = Input::GetInstance();
}

void ThirdPersonController::Update(float deltaTime) {
  if (!isActive_ || !camera_ || !primaryTarget_) {
    return;
  }

  // 標準FOVを設定
  if (camera_) {
    camera_->SetFovY(standardFov_);
  }

  ProcessInput(deltaTime);
  UpdateRotation();
  UpdatePosition();
}

void ThirdPersonController::Activate() {
  isActive_ = true;

  // 標準FOVを設定
  if (camera_) {
    camera_->SetFovY(standardFov_);
  }

  if (primaryTarget_) {
    Reset();
  }
}

void ThirdPersonController::Reset() {
  if (!primaryTarget_ || !camera_) {
    return;
  }

  // ターゲット位置に即座に移動
  interpolatedTargetPos_ = primaryTarget_->translate;

  // カメラをターゲットの向きに合わせる
  camera_->SetRotate(Vector3(0.0f, primaryTarget_->rotate.y, 0.0f));
  destinationAngleY_ = primaryTarget_->rotate.y;
  destinationAngleX_ = CameraConfig::FirstPerson::DEFAULT_ANGLE_X;
  destinationAngleZ_ = 0.0f;

  // オフセットをリセット
  offset_ = offsetOrigin_;

  // カメラ位置を更新
  Vector3 offset = CalculateOffset();
  camera_->SetTranslate(interpolatedTargetPos_ + offset);
}

void ThirdPersonController::ProcessInput(float deltaTime) {
  // ゲームパッド入力
  if (!input_->RStickInDeadZone()) {
    isRotating_ = true;
    float rotateX = input_->GetRightStick().x;
    destinationAngleY_ += rotateX * rotateSpeed_ *
      CameraConfig::FirstPerson::GAMEPAD_ROTATE_MULTIPLIER;
  } else {
    isRotating_ = false;
  }

  // 右スティック押し込みでターゲットの後ろにリセット
  if (input_->TriggerButton(XButtons.R_Thumbstick)) {
    destinationAngleY_ = primaryTarget_->rotate.y;
  }

  // キーボード入力
  if (input_->PushKey(DIK_LEFT)) {
    destinationAngleY_ -= rotateSpeed_;
  }
  if (input_->PushKey(DIK_RIGHT)) {
    destinationAngleY_ += rotateSpeed_;
  }
}

void ThirdPersonController::UpdateRotation() {
  // 現在の回転角度を取得
  Vector3 currentRotation = camera_->GetRotate();

  // 目標角度に向けて補間
  float angleY = Vec3::LerpShortAngle(currentRotation.y, destinationAngleY_, rotationLerpSpeed_);
  float angleX = Vec3::LerpShortAngle(currentRotation.x, destinationAngleX_, rotationLerpSpeed_);
  float angleZ = Vec3::LerpShortAngle(currentRotation.z, destinationAngleZ_, rotationLerpSpeed_);

  // カメラに適用
  camera_->SetRotate(Vector3(angleX, angleY, angleZ));
}

void ThirdPersonController::UpdatePosition() {
  // オフセットを補間
  offset_.z = Vec3::Lerp(offset_.z, offsetOrigin_.z, offsetLerpSpeed_);

  // ターゲット位置に補間して追従
  interpolatedTargetPos_ = Vec3::Lerp(interpolatedTargetPos_,
    primaryTarget_->translate,
    followSmoothness_);

  // カメラ位置を更新
  Vector3 offset = CalculateOffset();
  camera_->SetTranslate(interpolatedTargetPos_ + offset);
}

Vector3 ThirdPersonController::CalculateOffset() const {
  Vector3 offset = offset_;

  // カメラの回転行列を生成
  Matrix4x4 rotationMatrix = Mat4x4::MakeRotateXYZ(camera_->GetRotate());

  // オフセットを回転変換
  offset = Mat4x4::TransformNormal(rotationMatrix, offset);

  return offset;
}