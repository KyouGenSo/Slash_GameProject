#include "TitleScreen.h"
#include "Actor.h"
#include "GPUParticleEmitter.h"
#include "Input.h"
#include "ParticleSystemComponent.h"
#include "SceneManager.h"
#include "UILayout.h"
#include "UILayoutComponent.h"

#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#include "imgui.h"
#endif

namespace {

    constexpr BYTE kStartKey    = DIK_SPACE;
    constexpr WORD kStartButton = Tako::GamepadButton::A;

    Tako::UILayout* FindLayout(const Tako::Actor& actor) {
        Tako::UILayoutComponent* component = actor.GetComponent<Tako::UILayoutComponent>();
        return component ? &component->GetLayout() : nullptr;
    }

}

void TitleScreen::BeginPlay() {
    if (Tako::UILayout* layout = FindLayout(*GetOwner())) {
        layout->GetAnimator().Play(blinkClip_, blinkElement_);
    }
    ResetAnimation();
}

void TitleScreen::Tick(float deltaTime) {
    const Tako::Input* input = Tako::Input::GetInstance();
    if (!isAnimating_ && (input->TriggerKey(kStartKey) || input->TriggerButton(kStartButton))) {
        StartAnimation();
    }
    if (!isAnimating_) {
        return;
    }

    timer_ += deltaTime;
    const float progress = (std::min)(timer_ / duration_, 1.0f);
    // 2 回目からの呼び出しは、遷移の予約が済んでいるので無視される
    if (progress >= transitionProgress_) {
        Tako::SceneManager::GetInstance()->ChangeScene(nextScene_, Tako::TransitionManager::EffectType::Fade, fadeDuration_);
    }
    ApplySlash(progress);
    if (progress >= 1.0f) {
        isAnimating_ = false;
    }
}

void TitleScreen::StartAnimation() {
    if (Tako::UILayout* layout = FindLayout(*GetOwner())) {
        layout->GetAnimator().Play(startClip_);
    }
    isAnimating_ = true;
    timer_       = 0.0f;
}

void TitleScreen::ResetAnimation() {
    // 速さ 0 で再生すると、0 秒の姿勢（1 コマ目）のまま止まる
    if (Tako::UILayout* layout = FindLayout(*GetOwner())) {
        layout->GetAnimator().Play(startClip_, "", { .speed = 0.0f });
    }
    isAnimating_ = false;
    timer_       = 0.0f;
    ApplySlash(0.0f);
}

void TitleScreen::ApplySlash(float progress) {
    const Tako::Actor* actor = GetOwner()->Resolve(slashParticle_);
    Tako::ParticleSystemComponent* particle = actor ? actor->GetComponent<Tako::ParticleSystemComponent>() : nullptr;
    if (!particle) {
        return;
    }
    if (const std::shared_ptr<Tako::GPUParticleEmitter> emitter = particle->GetEmitter()) {
        emitter->SetParticleCount(static_cast<uint32_t>(std::lerp(static_cast<float>(startCount_), static_cast<float>(endCount_), progress)));
        emitter->SetFrequency(std::lerp(startFrequency_, endFrequency_, progress));
    }
}

#ifdef _DEBUG
bool TitleScreen::DrawCustomImGui() {
    if (ImGui::Button("Play Animation")) {
        StartAnimation();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Animation")) {
        ResetAnimation();
    }
    ImGui::Text("Animating: %s  (%.2f / %.2f s)", isAnimating_ ? "Yes" : "No", timer_, duration_);
    return false;
}
#endif
