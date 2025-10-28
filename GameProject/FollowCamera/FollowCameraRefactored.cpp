#include "FollowCameraRefactored.h"
#include "CameraAnimation/CameraAnimation.h"
#include "CameraSystem/FirstPersonController.h"
#include "CameraSystem/TopDownController.h"
#include "CameraSystem/CameraManager.h"
#include "CameraSystem/CameraConfig.h"
#include "FrameTimer.h"

#ifdef _DEBUG
#include "CameraSystem/CameraDebugUI.h"
#endif

FollowCamera::FollowCamera() {
    // CameraManagerのインスタンスを取得
    cameraManager_ = CameraManager::GetInstance();
}

FollowCamera::~FollowCamera() {
    Finalize();
}

void FollowCamera::Initialize(Camera* camera) {
    camera_ = camera;

    // CameraManagerを初期化
    cameraManager_->Initialize(camera);

    // カメラアニメーションを作成
    cameraAnimation_ = std::make_unique<CameraAnimation>();
    cameraAnimation_->SetCamera(camera);

    // コントローラーを初期化・登録
    InitializeControllers();

    // デフォルトモードを設定
    UpdateActiveController();
}

void FollowCamera::Finalize() {
    // アニメーションをクリーンアップ
    if (cameraAnimation_) {
        cameraAnimation_->Stop();
        cameraAnimation_.reset();
    }

    // マネージャーをクリーンアップ
    if (cameraManager_) {
        cameraManager_->DeactivateAllControllers();
    }

    camera_ = nullptr;
    primaryTarget_ = nullptr;
    secondaryTarget_ = nullptr;
    firstPersonController_ = nullptr;
    topDownController_ = nullptr;
}

void FollowCamera::Update() {
    if (!camera_) {
        return;
    }

    // アニメーションの更新
    float deltaTime = FrameTimer::GetInstance()->GetDeltaTime();
    cameraAnimation_->Update(deltaTime);

    // アニメーション再生中は通常のコントローラーを無効化
    if (cameraAnimation_->GetPlayState() == CameraAnimation::PlayState::PLAYING) {
        cameraManager_->DeactivateAllControllers();
        return;
    }

    // キーフレーム編集中の処理
    if (cameraAnimation_->IsEditingKeyframe()) {
        cameraAnimation_->ApplyKeyframeToCamera();
        cameraManager_->DeactivateAllControllers();
        return;
    }

    // 通常のカメラ更新
    cameraManager_->Update(deltaTime);
}

void FollowCamera::FirstPersonMode() {
    isFirstPersonMode_ = true;
    UpdateActiveController();
}

void FollowCamera::TopDownMode() {
    isFirstPersonMode_ = false;
    UpdateActiveController();
}

void FollowCamera::Reset() {
    if (isFirstPersonMode_ && firstPersonController_) {
        firstPersonController_->Reset();
    } else if (!isFirstPersonMode_ && topDownController_) {
        topDownController_->Reset();
    }
}

void FollowCamera::PlayStartCameraAnimation() {
    if (cameraAnimation_) {
        cameraAnimation_->LoadFromJson("game_start.json");
        cameraAnimation_->Play();
    }
}

void FollowCamera::DrawImGui() {
#ifdef _DEBUG
    // 新しいデバッグUIシステムを使用
    CameraDebugUI::Draw();

    // アニメーション専用UI
    if (cameraAnimation_) {
        CameraDebugUI::DrawAnimationInfo(cameraAnimation_.get());
    }
#endif
}

void FollowCamera::SetTarget(const Transform* target) {
    primaryTarget_ = target;

    // 各コントローラーにターゲットを設定
    if (firstPersonController_) {
        firstPersonController_->SetTarget(target);
    }
    if (topDownController_) {
        topDownController_->SetTarget(target);
    }

    // 現在のモードに応じてリセット
    Reset();
}

void FollowCamera::SetTarget2(const Transform* target2) {
    secondaryTarget_ = target2;

    // TopDownコントローラーに追加ターゲットを設定
    if (topDownController_) {
        std::vector<const Transform*> additionalTargets;
        if (target2) {
            additionalTargets.push_back(target2);
        }
        topDownController_->SetAdditionalTargets(additionalTargets);
    }
}

void FollowCamera::SetOffset(const Vector3& offset) {
    if (firstPersonController_) {
        firstPersonController_->SetOffset(offset);
    }
}

void FollowCamera::SetRotateSpeed(float speed) {
    if (firstPersonController_) {
        firstPersonController_->SetRotateSpeed(speed);
    }
}

void FollowCamera::SetMode(bool mode) {
    isFirstPersonMode_ = mode;
    UpdateActiveController();
    Reset();
}

Camera* FollowCamera::GetCamera() {
    return camera_;
}

const Matrix4x4& FollowCamera::GetViewProjection() const {
    if (camera_) {
        return camera_->GetViewProjectionMatrix();
    }

    static Matrix4x4 identity = Mat4x4::MakeIdentity();
    return identity;
}

Vector3 FollowCamera::GetOffset() const {
    if (firstPersonController_) {
        return firstPersonController_->GetOffset();
    }
    return Vector3{};
}

const Transform* FollowCamera::GetTarget2() const {
    return secondaryTarget_;
}

bool FollowCamera::GetMode() const {
    return isFirstPersonMode_;
}

void FollowCamera::InitializeControllers() {
    // FirstPersonControllerを作成・登録
    auto fpController = std::make_unique<FirstPersonController>();
    firstPersonController_ = fpController.get();
    cameraManager_->RegisterController("FirstPerson", std::move(fpController));

    // TopDownControllerを作成・登録
    auto tdController = std::make_unique<TopDownController>();
    topDownController_ = tdController.get();
    cameraManager_->RegisterController("TopDown", std::move(tdController));

    // ターゲットが既に設定されている場合は適用
    if (primaryTarget_) {
        SetTarget(primaryTarget_);
    }
    if (secondaryTarget_) {
        SetTarget2(secondaryTarget_);
    }
}

void FollowCamera::UpdateActiveController() {
    // すべてのコントローラーを非アクティブ化
    cameraManager_->DeactivateAllControllers();

    // 現在のモードに応じてアクティブ化
    if (isFirstPersonMode_) {
        cameraManager_->ActivateController("FirstPerson");
    } else {
        cameraManager_->ActivateController("TopDown");
    }
}