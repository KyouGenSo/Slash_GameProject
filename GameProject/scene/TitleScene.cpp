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

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugUIManager.h"
#endif

using namespace Tako;

namespace {
    constexpr float kFadeDuration = 0.2f;
    constexpr BYTE  kStartKey     = DIK_SPACE;
    constexpr WORD  kStartButton  = GamepadButton::A;

    // resources/Json/UI/Title.json の要素名
    constexpr const char* kBackgroundName  = "Background";
    constexpr const char* kTitleTextsName  = "TitleTexts";
    constexpr const char* kStartButtonName = "StartButton";

    // resources/Json/Animations/ のクリップ名（Tools > Animation Editor で編集する）
    constexpr const char* kStartBlinkClip = "TitleStartBlink";
    constexpr const char* kTitleSlashClip = "TitleSlash";  ///< タイトル文字のコマ送りと、そのあとの拡大フェードアウト
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

    UpdateSlashParticleAnimation();
    UpdateInput();

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

    // タイトル文字・拡大エフェクト・スタートボタンの動きは Tools > Animation Editor で編集する
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
    layout_.GetAnimator().Play(kTitleSlashClip);

    isSlashEmitterAnimating_ = true;
    slashEmitterAnimTimer_ = 0.0f;

}

void TitleScene::StopTitleAnimation()
{
    // 速さ 0 で再生すると 0 秒の姿勢（1 コマ目、拡大エフェクト非表示）のまま止まる
    layout_.GetAnimator().Play(kTitleSlashClip, "", { .speed = 0.0f });
}

void TitleScene::ResetTitleAnimation()
{
    StopTitleAnimation();

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
    layout_.GetAnimator().Play(kStartBlinkClip, kStartButtonName);

    // 押されるまではタイトル文字を 1 コマ目で止めておく
    StopTitleAnimation();
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

void TitleScene::UpdateInput()
{
    if (Input::GetInstance()->TriggerKey(kStartKey) ||
        Input::GetInstance()->TriggerButton(kStartButton)) {
        if (!isSlashEmitterAnimating_) {
            PlayTitleAnimation();
        }
    }
}
