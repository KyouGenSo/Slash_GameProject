#include "GameCamera.h"
#include "Actor.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "Renderer.h"

#include "Source/CameraSystem/CameraManager.h"
#include "Source/CameraSystem/Controller/CameraAnimationController.h"
#include "Source/CameraSystem/Controller/ThirdPersonController.h"
#include "Source/CameraSystem/Controller/TopDownController.h"

#ifdef _DEBUG
#include "Source/CameraSystem/CameraDebugUI.h"
#endif

#include <memory>
#include <vector>

void GameCamera::BeginPlay() {
    Tako::Actor* target    = GetOwner()->Resolve(target_);
    Tako::Actor* secondary = GetOwner()->Resolve(secondaryTarget_);
    if (!target || !secondary) {
        return;
    }
    CameraManager* manager = CameraManager::GetInstance();
    manager->Initialize(*Tako::Renderer::GetInstance()->GetCamera());

    auto thirdPerson = std::make_unique<ThirdPersonController>();
    thirdPerson->SetTarget(&target->GetTransform());
    thirdPerson->SetSecondaryTarget(&secondary->GetTransform());
    thirdPerson->EnableLookAtTarget(true);
    thirdPersonController_ = thirdPerson.get();
    manager->RegisterController("ThirdPerson", std::move(thirdPerson));

    auto topDown = std::make_unique<TopDownController>();
    topDown->SetTarget(&target->GetTransform());
    topDown->SetAdditionalTargets(std::vector<const Tako::Transform*>{ &secondary->GetTransform() });
    topDownController_ = topDown.get();
    manager->RegisterController("TopDown", std::move(topDown));

    auto animation = std::make_unique<CameraAnimationController>();
    animationController_ = animation.get();
    manager->RegisterController("Animation", std::move(animation));

    ApplySettings(true);

    // 開始の演出を再生し、終わりの演出を読んでおく（それぞれプレイヤーとボスを基準に動く）
    animationController_->LoadAnimationFromFile(startAnimation_);
    manager->ActivateController("Animation");
    animationController_->SwitchAnimation(startAnimation_);
    animationController_->Play();

    animationController_->LoadAnimationFromFile(overAnimation_);
    animationController_->SetAnimationTargetByName(overAnimation_, &target->GetTransform());

    animationController_->LoadAnimationFromFile(clearAnimation_);
    animationController_->SetAnimationTargetByName(clearAnimation_, &secondary->GetTransform());
}

void GameCamera::EndPlay() {
    if (!animationController_) {
        return;
    }
    // コントローラーは CameraManager が消す
    CameraManager::GetInstance()->Finalize();
    thirdPersonController_ = nullptr;
    topDownController_     = nullptr;
    animationController_   = nullptr;
}

void GameCamera::LateTick(float deltaTime) {
    if (!animationController_) {
        return;
    }
    CameraManager::GetInstance()->Update(deltaTime);

    // CameraManager は描画に使うカメラを直接動かす。その結果を Actor に写しておくと、CameraComponent が同じ値を書き戻す
    const Tako::Camera* camera = *Tako::Renderer::GetInstance()->GetCamera();
    GetOwner()->GetTransform().translate = camera->GetTranslate();
    GetOwner()->GetTransform().rotate    = camera->GetRotate();
    if (Tako::CameraComponent* component = GetOwner()->GetComponent<Tako::CameraComponent>()) {
        component->SetFovY(camera->GetFovY());
    }
}

void GameCamera::PostEditChange() {
    ApplySettings(false);
}

void GameCamera::ApplySettings(bool resetsCurrent) {
    if (thirdPersonController_) {
        thirdPersonController_->ApplySettings(thirdPerson_);
    }
    if (topDownController_) {
        topDownController_->ApplySettings(topDown_, resetsCurrent);
    }
}

#ifdef _DEBUG
bool GameCamera::DrawCustomImGui() {
    if (animationController_) {
        CameraDebugUI::Draw();
    }
    return false;
}
#endif
