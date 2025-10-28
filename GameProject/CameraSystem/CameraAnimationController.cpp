#include "CameraAnimationController.h"

CameraAnimationController::CameraAnimationController() {
    animation_ = std::make_unique<CameraAnimation>();
}

void CameraAnimationController::Update(float deltaTime) {
    if (!animation_ || !camera_) {
        return;
    }

    // アニメーション更新
    animation_->Update(deltaTime);

    // 再生完了時の自動非アクティブ化
    if (autoDeactivateOnComplete_) {
        auto state = animation_->GetPlayState();
        if (state == CameraAnimation::PlayState::STOPPED &&
            !animation_->IsLooping()) {
            // ワンショット再生が完了したら自動的に非アクティブ化
            isActive_ = false;
        }
    }
}

bool CameraAnimationController::IsActive() const {
    if (!animation_) {
        return false;
    }

    // アニメーション再生中または編集中の場合はアクティブ
    return animation_->GetPlayState() == CameraAnimation::PlayState::PLAYING ||
           animation_->IsEditingKeyframe() ||
           isActive_;
}

void CameraAnimationController::Activate() {
    isActive_ = true;
    if (animation_) {
        animation_->Play();
    }
}

void CameraAnimationController::Deactivate() {
    isActive_ = false;
    if (animation_) {
        animation_->Stop();
    }
}

void CameraAnimationController::SetCamera(Camera* camera) {
    ICameraController::SetCamera(camera);
    if (animation_) {
        animation_->SetCamera(camera);
    }
}

bool CameraAnimationController::LoadAnimation(const std::string& filepath) {
    if (!animation_) {
        return false;
    }
    return animation_->LoadFromJson(filepath);
}

void CameraAnimationController::Play() {
    if (animation_) {
        animation_->Play();
        isActive_ = true;
    }
}

void CameraAnimationController::Pause() {
    if (animation_) {
        animation_->Pause();
    }
}

void CameraAnimationController::Stop() {
    if (animation_) {
        animation_->Stop();
        isActive_ = false;
    }
}

void CameraAnimationController::Reset() {
    if (animation_) {
        animation_->Reset();
    }
}

void CameraAnimationController::AddKeyframe(const CameraKeyframe& keyframe) {
    if (animation_) {
        animation_->AddKeyframe(keyframe);
    }
}

void CameraAnimationController::AddKeyframeFromCurrentCamera(float time,
    CameraKeyframe::InterpolationType interpolation) {
    if (animation_) {
        animation_->AddKeyframeFromCurrentCamera(time, interpolation);
    }
}

void CameraAnimationController::RemoveKeyframe(size_t index) {
    if (animation_) {
        animation_->RemoveKeyframe(index);
    }
}

void CameraAnimationController::ClearKeyframes() {
    if (animation_) {
        animation_->ClearKeyframes();
    }
}

void CameraAnimationController::SetLooping(bool loop) {
    if (animation_) {
        animation_->SetLooping(loop);
    }
}

void CameraAnimationController::SetPlaySpeed(float speed) {
    if (animation_) {
        animation_->SetPlaySpeed(speed);
    }
}

void CameraAnimationController::SetAnimationName(const std::string& name) {
    if (animation_) {
        animation_->SetAnimationName(name);
    }
}

CameraAnimation::PlayState CameraAnimationController::GetPlayState() const {
    if (animation_) {
        return animation_->GetPlayState();
    }
    return CameraAnimation::PlayState::STOPPED;
}

float CameraAnimationController::GetDuration() const {
    if (animation_) {
        return animation_->GetDuration();
    }
    return 0.0f;
}

float CameraAnimationController::GetCurrentTime() const {
    if (animation_) {
        return animation_->GetCurrentTime();
    }
    return 0.0f;
}

bool CameraAnimationController::IsEditingKeyframe() const {
    if (animation_) {
        return animation_->IsEditingKeyframe();
    }
    return false;
}