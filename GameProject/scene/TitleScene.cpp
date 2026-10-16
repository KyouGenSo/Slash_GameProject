#include "TitleScene.h"

#include "SceneManager.h"
#include "Object3dBasic.h"
#include "SpriteBasic.h"
#include "ModelManager.h"
#include "Input.h"
#include "FrameTimer.h"
#include "LineRenderer.h"
#include "GPUParticle.h"
#include "Vec3Func.h"
#include <cmath>
#include <numbers>
#include <format>

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugUIManager.h"
#endif

using namespace Tako;

namespace {
    constexpr float kFadeDuration    = 0.2f;
    constexpr int   kTitleTextCount  = 10;
    constexpr float kBlinkTimerLimit = 1000.0f;
    constexpr BYTE  kStartKey        = DIK_SPACE;
    constexpr WORD  kStartButton     = GamepadButton::A;

    // resources/Json/UI/Title.json の要素名
    constexpr const char* kBackgroundName  = "Background";
    constexpr const char* kTitleTextsName  = "TitleTexts";
    constexpr const char* kStartButtonName = "StartButton";
    constexpr const char* kTitleEffectName = "TitleTextEffect";

    /// <summary>
    /// フレームアニメーションの index 番目（0 始まり）の要素名
    /// </summary>
    std::string TitleTextName(int index) { return std::format("TitleText{}", index + 1); }
}

void TitleScene::Initialize()
{
    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///

    emitterManager_ = std::make_unique<EmitterManager>(GPUParticle::GetInstance());

    // シーンプリセット統合保存のため連携
    forceFieldManager_ = std::make_unique<ForceFieldManager>(GPUParticle::GetInstance());
    emitterManager_->SetForceFieldManager(forceFieldManager_.get());

    InitializeDebugUI();
    InitializeCamera();
    InitializePostEffects();
    InitializeUI();
    InitializeParticles();
}

void TitleScene::Finalize()
{
#ifdef _DEBUG
    DebugUIManager::GetInstance()->ClearGameObjects();
#endif

    PostEffectManager::GetInstance()->ClearEffectChain();
}

void TitleScene::Update()
{
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    UpdateStartButtonBlink();
    UpdateTitleTextAnimation();
    UpdateSlashParticleAnimation();
    UpdateTitleEffectAnimation();
    UpdateInput();

    // タイトルテキストは今のフレームの 1 枚だけ、拡大エフェクトは再生中だけ表示する
    for (int i = 0; i < kTitleTextCount; ++i) {
        layout_.SetVisible(TitleTextName(i), i == currentFrame_);
    }
    layout_.SetVisible(kTitleEffectName, isEffectPlaying_);
    layout_.Update();

    emitterManager_->Update();
}

void TitleScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    layout_.Draw(kBackgroundName);


    //-------------------Model の描画-------------------//
    Object3dBasic::GetInstance()->SetCommonRenderSetting();




    //------------------前景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    // 拡大エフェクトは TitleTexts の最後の要素なので、タイトルテキストの上に描画される
    layout_.Draw(kTitleTextsName);

}

void TitleScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();




    //-------------------Model の描画-------------------//
    Object3dBasic::GetInstance()->SetCommonRenderSetting();





    //------------------前景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    layout_.Draw(kStartButtonName);

}

void TitleScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGui の描画              ///
    /// ================================== ///

    if (ImGui::Button("Play Animation")) {
        PlayTitleAnimation();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Animation")) {
        StopTitleAnimation();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset Animation")) {
        ResetTitleAnimation();
    }

    ImGui::Separator();
    ImGui::Text("Animation Settings");
    ImGui::SliderInt("Animation Speed", &animationSpeed_, 1, 30);
    ImGui::Checkbox("Loop", &isLoop_);

    ImGui::Separator();
    ImGui::Text("Status");
    ImGui::Text("Current Frame: %d / %d", currentFrame_ + 1, kTitleTextCount);
    ImGui::Text("Is Playing: %s", isPlaying_ ? "Yes" : "No");
    ImGui::Text("Animation Complete: %s", animationComplete_ ? "Yes" : "No");

    ImGui::Separator();
    ImGui::Text("Manual Frame Control");
    if (ImGui::SliderInt("Frame", &currentFrame_, 0, kTitleTextCount - 1)) {
        // 手動変更時はアニメーションを停止
        isPlaying_ = false;
    }

    ImGui::Checkbox("Enable Blinking", &isButtonBlinking_);

    ImGui::Separator();
    ImGui::Text("Blink Parameters");
    ImGui::SliderFloat("Blink Speed", &blinkSpeed_, 0.5f, 10.0f, "%.1f");
    ImGui::SliderFloat("Min Alpha", &blinkMinAlpha_, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Max Alpha", &blinkMaxAlpha_, 0.0f, 1.0f, "%.2f");

    if (ImGui::Button("Reset Timer")) {
        blinkTimer_ = 0.0f;
    }

    ImGui::Separator();
    ImGui::Text("Current Status");
    ImGui::Text("Timer: %.2f", blinkTimer_);
    ImGui::Text("Current Alpha: %.2f", layout_.GetAlpha(kStartButtonName));

    ImGui::Separator();
    ImGui::Text("Presets");
    if (ImGui::Button("Slow Fade")) {
        blinkSpeed_ = 1.0f;
        blinkMinAlpha_ = 0.3f;
        blinkMaxAlpha_ = 1.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Fast Blink")) {
        blinkSpeed_ = 5.0f;
        blinkMinAlpha_ = 0.0f;
        blinkMaxAlpha_ = 1.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Gentle Pulse")) {
        blinkSpeed_ = 2.0f;
        blinkMinAlpha_ = 0.5f;
        blinkMaxAlpha_ = 1.0f;
    }

    ImGui::Separator();
    ImGui::Text("Title Text Expansion Effect");

    if (ImGui::Button("Trigger Effect")) {
        isEffectPlaying_ = true;
        effectTriggered_ = false;  // 再度トリガー可能にする
        effectTimer_ = 0.0f;
        effectScale_ = 1.0f;
        effectAlpha_ = effectInitialAlpha_;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Effect")) {
        isEffectPlaying_ = false;
        effectAlpha_ = 0.0f;
    }

    ImGui::SliderFloat("Effect Duration", &effectDuration_, 0.5f, 5.0f, "%.1f sec");
    ImGui::SliderFloat("Max Scale", &effectMaxScale_, 1.0f, 3.0f, "%.1f");
    ImGui::SliderFloat("Initial Alpha", &effectInitialAlpha_, 0.0f, 1.0f, "%.2f");

    ImGui::Text("Effect Status");
    ImGui::Text("Is Playing: %s", isEffectPlaying_ ? "Yes" : "No");
    ImGui::Text("Timer: %.2f / %.2f", effectTimer_, effectDuration_);
    ImGui::Text("Current Scale: %.2f", effectScale_);
    ImGui::Text("Current Alpha: %.2f", effectAlpha_);

    if (ImGui::Button("Reset All")) {
        ResetTitleAnimation();
        isEffectPlaying_ = false;
        effectTriggered_ = false;
        effectTimer_ = 0.0f;
        effectScale_ = 1.0f;
        effectAlpha_ = 0.0f;
        layout_.SetAlpha(kTitleEffectName, 0.0f);
    }

    ImGui::Separator();
    ImGui::Text("Slash Particle Animation");

    if (ImGui::Button("Start Particle Animation")) {
        isSlashEmitterAnimating_ = true;
        slashEmitterAnimTimer_ = 0.0f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop Particle Animation")) {
        isSlashEmitterAnimating_ = false;
    }

    ImGui::Text("Start Values:");
    int startCount = static_cast<int>(slashEmitterStartCount_);
    if (ImGui::SliderInt("Start Count", &startCount, 1, 200)) {
        slashEmitterStartCount_ = static_cast<uint32_t>(startCount);
    }
    ImGui::SliderFloat("Start Frequency", &slashEmitterStartFreq_, 0.001f, 1.0f, "%.3f");

    ImGui::Text("End Values:");
    int endCount = static_cast<int>(slashEmitterEndCount_);
    if (ImGui::SliderInt("End Count", &endCount, 1, 300)) {
        slashEmitterEndCount_ = static_cast<uint32_t>(endCount);
    }
    ImGui::SliderFloat("End Frequency", &slashEmitterEndFreq_, 0.001f, 1.0f, "%.3f");

    ImGui::Text("Current Status:");
    ImGui::Text("Is Animating: %s", isSlashEmitterAnimating_ ? "Yes" : "No");
    ImGui::Text("Timer: %.2f / %.2f", slashEmitterAnimTimer_, slashEmitterAnimDuration_);

    auto slashEmitter = emitterManager_->GetEmitterByName("slash");
    if (slashEmitter) {
        ImGui::Text("Current Count: %u", slashEmitter->GetParticleCount());
        ImGui::Text("Current Frequency: %.3f", slashEmitter->GetFrequency());
    }

#endif // _DEBUG
}

void TitleScene::PlayTitleAnimation()
{
    isPlaying_ = true;
    animationComplete_ = false;

    isSlashEmitterAnimating_ = true;
    slashEmitterAnimTimer_ = 0.0f;

}

void TitleScene::StopTitleAnimation()
{
    isPlaying_ = false;
}

void TitleScene::ResetTitleAnimation()
{
    currentFrame_ = 0;
    frameCounter_ = 0.0f;
    isPlaying_ = false;
    animationComplete_ = false;

    isSlashEmitterAnimating_ = false;
    slashEmitterAnimTimer_ = 0.0f;

    // slash エミッターを初期値に戻す
    auto slashEmitter = emitterManager_->GetEmitterByName("slash");
    if (slashEmitter) {
        slashEmitter->SetParticleCount(slashEmitterStartCount_);
        slashEmitter->SetFrequency(slashEmitterStartFreq_);
    }
}

void TitleScene::InitializeDebugUI()
{
#ifdef _DEBUG
    // 背景・タイトルテキスト・スタートボタンの配置は Tools > UI Editor で編集する
    DebugUIManager::GetInstance()->RegisterGameObject("TitleScene",
        [this]() { this->DrawImGui(); });

    DebugUIManager::GetInstance()->SetEmitterManager(emitterManager_.get());
    DebugUIManager::GetInstance()->SetForceFieldManager(forceFieldManager_.get());
#endif
}

void TitleScene::InitializeCamera()
{
    (*Object3dBasic::GetInstance()->GetCamera())->SetRotate(Vector3(0.2f, 0.0f, 0.0f));
    (*Object3dBasic::GetInstance()->GetCamera())->SetTranslate(Vector3(0.0f, cameraY_ + offsetY_, cameraZ_));
}

void TitleScene::InitializePostEffects()
{
    PostEffectManager::GetInstance()->LoadProfile("title");
}

void TitleScene::InitializeUI()
{
    layout_.Load("Title");
}

void TitleScene::InitializeParticles()
{
    emitterManager_->LoadScenePreset("title_preset");

    auto slashEmitter = emitterManager_->GetEmitterByName("slash");
    if (slashEmitter) {
        slashEmitter->SetParticleCount(slashEmitterStartCount_);
        slashEmitter->SetFrequency(slashEmitterStartFreq_);
    }
}

void TitleScene::UpdateStartButtonBlink()
{
    if (!isButtonBlinking_) return;

    blinkTimer_ += FrameTimer::GetInstance()->GetDeltaTime();
    if (blinkTimer_ > kBlinkTimerLimit) blinkTimer_ = 0.f; // オーバーフロー防止

    // sin の結果(-1〜1)を0〜1に正規化し、min〜max にマッピング
    float sineValue = std::sin(blinkTimer_ * blinkSpeed_ * std::numbers::pi_v<float>);
    float normalizedSine = (sineValue + 1.0f) * 0.5f;
    float alpha = blinkMinAlpha_ + (blinkMaxAlpha_ - blinkMinAlpha_) * normalizedSine;

    layout_.SetAlpha(kStartButtonName, alpha);
}

void TitleScene::UpdateTitleTextAnimation()
{
    if (!isPlaying_) return;

    frameCounter_ += FrameRate::Frames(FrameTimer::GetInstance()->GetDeltaTime());

    if (frameCounter_ >= static_cast<float>(animationSpeed_)) {
        // 端数を持ち越して切り替え間隔を FPS に依らず保つ
        frameCounter_ = std::fmod(frameCounter_, static_cast<float>(animationSpeed_));
        currentFrame_++;

        if (currentFrame_ >= kTitleTextCount) {
            if (isLoop_) {
                currentFrame_ = 0;
            }
            else {
                // 最後のフレームで停止
                currentFrame_ = kTitleTextCount - 1;
                isPlaying_ = false;
                animationComplete_ = true;
            }
        }
    }

    // 完了時にエフェクトを一度だけ開始
    if (animationComplete_ && !effectTriggered_ && !isLoop_) {
        isEffectPlaying_ = true;
        effectTriggered_ = true;
        effectTimer_ = 0.0f;
        effectScale_ = 1.0f;
        effectAlpha_ = effectInitialAlpha_;
    }
}

void TitleScene::UpdateSlashParticleAnimation()
{
    if (!isSlashEmitterAnimating_) return;

    slashEmitterAnimTimer_ += FrameTimer::GetInstance()->GetDeltaTime();

    float progress = slashEmitterAnimTimer_ / slashEmitterAnimDuration_;

    if (progress >= sceneTransitionProgress_) {
        SceneManager::GetInstance()->ChangeScene("game", TransitionManager::EffectType::Fade, kFadeDuration);
    }

    if (progress >= 1.0f) {
        progress = 1.0f;
        isSlashEmitterAnimating_ = false;
    }

    auto slashEmitter = emitterManager_->GetEmitterByName("slash");
    if (slashEmitter) {
        uint32_t currentCount = static_cast<uint32_t>(
            Vec3::Lerp(
                static_cast<float>(slashEmitterStartCount_),
                static_cast<float>(slashEmitterEndCount_),
                progress));

        float currentFreq = Vec3::Lerp(slashEmitterStartFreq_, slashEmitterEndFreq_, progress);

        slashEmitter->SetParticleCount(currentCount);
        slashEmitter->SetFrequency(currentFreq);
    }
}

void TitleScene::UpdateTitleEffectAnimation()
{
    if (!isEffectPlaying_) return;

    effectTimer_ += FrameTimer::GetInstance()->GetDeltaTime();

    float progress = effectTimer_ / effectDuration_;

    if (progress >= 1.0f) {
        progress = 1.0f;
        isEffectPlaying_ = false;
        effectAlpha_ = 0.0f;
    }
    else {
        effectScale_ = Vec3::Lerp(1.0f, effectMaxScale_, progress);

        effectAlpha_ = Vec3::Lerp(effectInitialAlpha_, 0.0f, progress);
    }

    // 基準の大きさはゲームが書き換えない最後のフレームから取る。pivot が中央なので中心を保って拡大される
    layout_.SetSize(kTitleEffectName, layout_.GetSize(TitleTextName(kTitleTextCount - 1)) * effectScale_);
    layout_.SetAlpha(kTitleEffectName, effectAlpha_);
}

void TitleScene::UpdateInput()
{
    if (Input::GetInstance()->TriggerKey(kStartKey) ||
        Input::GetInstance()->TriggerButton(kStartButton)) {
        if (!isPlaying_ && !isSlashEmitterAnimating_) {
            PlayTitleAnimation();
        }
    }
}