#include "TopDownController.h"
#include "Vec3Func.h"
#include <algorithm>
#include <numeric>

using namespace Tako;

TopDownController::TopDownController() {
    // デフォルト値設定
    currentHeight_ = 88.f;
    currentBackOffset_ = -205.f;
}

void TopDownController::Update(float deltaTime) {
    if (!isActive_ || !camera_ || !primaryTarget_) {
        return;
    }

    // 標準FOVを設定
    if (camera_) {
      camera_->SetFovY(standardFov_);
    }

    UpdateCameraPosition();
}

void TopDownController::Activate() {
    isActive_ = true;

    // 標準FOVを設定
    if (camera_) {
      camera_->SetFovY(standardFov_);
    }

    if (primaryTarget_) {
        Reset();
    }
}

void TopDownController::Reset() {
    if (!primaryTarget_ || !camera_) {
        return;
    }

    // ターゲット位置に即座に移動
    interpolatedTargetPos_ = CalculateFocusPoint();

    // カメラの位置と角度を設定
    UpdateCameraPosition();
}

Vector3 TopDownController::CalculateFocusPoint() const {
    if (!primaryTarget_) {
        return Vector3{};
    }

    // 全ターゲットの中心を計算
    std::vector<Vector3> allPositions;
    allPositions.push_back(primaryTarget_->translate);

    for (const auto* target : additionalTargets_) {
        if (target) {
            allPositions.push_back(target->translate);
        }
    }

    // 平均位置を計算
    if (!allPositions.empty()) {
        Vector3 sum = std::accumulate(allPositions.begin(), allPositions.end(),
                                    Vector3{0.0f, 0.0f, 0.0f},
                                    [](const Vector3& a, const Vector3& b) {
                                        return Vec3::Add(a, b);
                                    });
        return Vec3::Multiply(sum, 1.0f / static_cast<float>(allPositions.size()));
    }

    return primaryTarget_->translate;
}

float TopDownController::CalculateMaxTargetDistance() const {
    if (!primaryTarget_ || additionalTargets_.empty()) {
        return 0.0f;
    }

    float maxDistance = 0.0f;

    // プライマリターゲットと各追加ターゲット間の距離を計算
    for (const auto* target : additionalTargets_) {
        if (target) {
            Vector3 diff = Vec3::Subtract(primaryTarget_->translate, target->translate);
            float distance = static_cast<float>(Vec3::Length(diff));
            maxDistance = std::max(maxDistance, distance);
        }
    }

    // 追加ターゲット同士の距離も考慮
    for (size_t i = 0; i < additionalTargets_.size(); ++i) {
        for (size_t j = i + 1; j < additionalTargets_.size(); ++j) {
            if (additionalTargets_[i] && additionalTargets_[j]) {
                Vector3 diff = Vec3::Subtract(additionalTargets_[i]->translate,
                                            additionalTargets_[j]->translate);
                float distance = static_cast<float>(Vec3::Length(diff));
                maxDistance = std::max(maxDistance, distance);
            }
        }
    }

    return maxDistance;
}

void TopDownController::CalculateCameraParameters(float targetDistance,
                                                 float& outHeight,
                                                 float& outBackOffset) const {
    // 距離に応じて高度を調整
    outHeight = baseHeight_ + targetDistance * heightMultiplier_;
    outHeight = std::clamp(outHeight, minHeight_, maxHeight_);

    // 距離に応じて後方オフセットも調整
    outBackOffset = baseBackOffset_ - targetDistance * backOffsetMultiplier_;
    outBackOffset = std::clamp(outBackOffset, minBackOffset_, maxBackOffset_);
}

void TopDownController::UpdateCameraPosition() {
    // フォーカスポイントを計算
    Vector3 focusPoint = CalculateFocusPoint();

    // 滑らかに追従
    interpolatedTargetPos_ = Vec3::Lerp(interpolatedTargetPos_, focusPoint, followSmoothness_);

    // ターゲット間の距離を計算
    float targetDistance = CalculateMaxTargetDistance();

    // カメラパラメータを計算
    float targetHeight = baseHeight_;
    float targetBackOffset = baseBackOffset_;

    if (targetDistance > 0.0f) {
        CalculateCameraParameters(targetDistance, targetHeight, targetBackOffset);
    }

    // 現在の値を滑らかに更新
    currentHeight_ = Vec3::Lerp(currentHeight_, targetHeight, followSmoothness_);
    currentBackOffset_ = Vec3::Lerp(currentBackOffset_, targetBackOffset, followSmoothness_);

    // カメラ位置を設定
    Vector3 cameraPos = interpolatedTargetPos_;
    cameraPos.y = currentHeight_;
    cameraPos.z += currentBackOffset_;

    camera_->SetTranslate(cameraPos);

    // カメラの回転を固定（俯瞰角度）
    camera_->SetRotate(Vector3(cameraAngleX_, 0.0f, 0.0f));
}