#include "ResultScreen.h"
#include "Actor.h"
#include "Input.h"
#include "SceneManager.h"
#include "UILayout.h"
#include "UILayoutComponent.h"

void ResultScreen::BeginPlay() {
    Tako::UILayoutComponent* layout = GetOwner()->GetComponent<Tako::UILayoutComponent>();
    if (layout && !titleTexture_.empty()) {
        layout->GetLayout().SetTexture(titleElement_, titleTexture_);
    }
}

void ResultScreen::Tick([[maybe_unused]] float deltaTime) {
    const Tako::Input* input = Tako::Input::GetInstance();
    if (input->TriggerKey(DIK_SPACE) || input->TriggerButton(Tako::GamepadButton::A)) {
        Tako::SceneManager::GetInstance()->ChangeScene(nextScene_);
    }
}
