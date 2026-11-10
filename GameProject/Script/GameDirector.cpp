#include "GameDirector.h"

// Engine includes
#include "Actor.h"
#include "GameCamera.h"
#include "BaseScene.h"
#include "Logger.h"
#include "UILayoutComponent.h"
#include "WinApp.h"
#include "Renderer.h"
#include "Input.h"
#include "LineRenderer.h"
#include "FrameTimer.h"
#include "GPUParticle.h"
#include "SceneManager.h"
#include "EmitterManager.h"
#include "ForceFieldManager.h"
#include "Model.h"
#include "CollisionManager.h"
#include "PostEffectManager.h"
#include "DecalManager.h"

// Game includes
#include "CameraSystem/CameraManager.h"
#include "CameraSystem/Controller/CameraAnimationController.h"
#include "Object/Player/State/PlayerState.h"
#include "Object/Player/State/PlayerStateMachine.h"
#include "Common/GameConst.h"

#include <algorithm>
#include <cmath>

using namespace Tako;

namespace {
    constexpr BYTE  kCameraModeToggleKey = DIK_P;
    constexpr float kFadeDuration        = 0.3f;
    constexpr int   kVortexEmitterCount  = 4;
}

void GameDirector::BeginPlay()
{
    Tako::Actor* playerActor = GetOwner()->Resolve(playerActor_);
    Tako::Actor* bossActor   = GetOwner()->Resolve(bossActor_);
    Tako::Actor* cameraActor = GetOwner()->Resolve(cameraActor_);
    GameCamera*  gameCamera  = cameraActor ? cameraActor->GetScript<GameCamera>() : nullptr;
    player_ = playerActor ? playerActor->GetScript<Player>() : nullptr;
    boss_   = bossActor ? bossActor->GetScript<Boss>() : nullptr;
    // GameCamera は Order が小さいので、BeginPlay が済んでいる
    gameCamera_          = gameCamera;
    animationController_ = gameCamera ? gameCamera->GetAnimationController() : nullptr;
    Tako::Actor* controllerUiActor = GetOwner()->Resolve(controllerUiActor_);
    Tako::Actor* pauseMenuActor    = GetOwner()->Resolve(pauseMenuActor_);
    controllerUI_    = controllerUiActor ? controllerUiActor->GetScript<ControllerUI>() : nullptr;
    pauseMenu_       = pauseMenuActor ? pauseMenuActor->GetScript<PauseMenu>() : nullptr;
    pauseMenuLayout_ = pauseMenuActor ? pauseMenuActor->GetComponent<Tako::UILayoutComponent>() : nullptr;
    if (!player_ || !boss_ || !animationController_ || !controllerUI_ || !pauseMenu_ || !pauseMenuLayout_) {
        Tako::Logger::Log("GameDirector: an Actor reference (Player / Boss / Camera / Controller UI / Pause Menu) is not set, or the Actor lacks the expected script or UILayoutComponent\n");
        player_ = nullptr;
        boss_   = nullptr;
        return;
    }
    cameraManager_ = CameraManager::GetInstance();

    Tako::BaseScene* scene = GetOwner()->GetScene();
    emitterManager_    = &scene->GetEmitterManager();
    forceFieldManager_ = &scene->GetForceFieldManager();

    inputHandler_ = std::make_unique<InputHandler>();
    inputHandler_->Initialize();

    pauseMenuLayout_->SetVisible(false);

    InitializeObject3d();


    InitializeEmitterManager();

    InitializeEffectManager();

}

void GameDirector::EndPlay()
{
    // コードが作った当たり判定（BT ノードの物）の残りを消す。コライダーは Awake で登録されるので、BeginPlay では消さない
    CollisionManager::GetInstance()->Reset();

    DecalManager::GetInstance()->ClearDecals();
}

void GameDirector::Tick([[maybe_unused]] float deltaTime)
{
    if (!player_) {
        return;
    }

#ifdef _DEBUG
    // P キーでカメラモード切り替え
    if (Input::GetInstance()->TriggerKey(kCameraModeToggleKey)) {
        cameraMode_ = !cameraMode_;
    }

#endif

    // ポーズトグル判定の前に実行する必要があるため、ポーズ中でも更新する
    inputHandler_->Update();

    CheckPause();

    if (isPaused_) {
        UpdatePause();
        return;
    }

    UpdateGameFlow();

    UpdateCameraMode();

    UpdateInput();
}

void GameDirector::LateTick([[maybe_unused]] float deltaTime)
{
    if (!player_ || isPaused_) {
        return;
    }

    UpdateEffects();

    // 当たり判定と一時エミッターの寿命は、この後に BaseScene の既定の Update が行う
}

void GameDirector::UpdateGameFlow()
{
    // 開始演出が終わったらボスの一時停止を解除
    if (animationController_->GetPlayState() != CameraAnimation::PlayState::PLAYING && !isStart_) {
        isStart_ = true;
        boss_->SetIsPause(false);
    }

    // ゲームクリア判定と演出開始
    if (boss_->IsDead() && !clearEffectManager_->IsPlaying() && !clearEffectManager_->IsComplete()) {
        cameraManager_->DeactivateAllControllers();
        cameraManager_->ActivateController("Animation");
        animationController_->SwitchAnimation(gameCamera_->GetClearAnimation());
        animationController_->Play();
        boss_->SetIsPause(true);
        player_->SetScale(Vector3(0.f, 0.f, 0.f)); // プレイヤーを非表示
        clearEffectManager_->Start();
    }

    // ゲームオーバー判定と演出開始
    if (player_->IsDead() && !overEffectManager_->IsPlaying() && !overEffectManager_->IsComplete()) {
        cameraManager_->DeactivateAllControllers();
        cameraManager_->ActivateController("Animation");
        animationController_->SwitchAnimation(gameCamera_->GetOverAnimation());
        animationController_->Play();
        boss_->SetIsPause(true);
        overEffectManager_->Start();
    }
}

void GameDirector::UpdateEffects()
{
    float deltaTime = FrameTimer::GetInstance()->GetDeltaTime();

    bool isDashing = false;
    if (player_ && player_->GetStateMachine() && player_->GetStateMachine()->GetCurrentState()) {
        isDashing = (player_->GetStateMachine()->GetCurrentState()->GetName() == "Dash");
    }
    dashEffectManager_->Update(deltaTime, player_->GetTranslate(), isDashing);

    bossBorderManager_->Update(boss_->GetPhase(), boss_->GetTranslate());

    overEffectManager_->Update(deltaTime);
    if (overEffectManager_->IsComplete()) {
        SceneManager::GetInstance()->ChangeScene(overScene_, TransitionManager::EffectType::Fade, kFadeDuration);
    }

    clearEffectManager_->Update(deltaTime);
    if (clearEffectManager_->IsComplete()) {
        SceneManager::GetInstance()->ChangeScene(clearScene_, TransitionManager::EffectType::Fade, kFadeDuration);
    }
}

void GameDirector::Draw()
{
#ifdef _DEBUG
    // Edit 中は BeginPlay が呼ばれていないので、ゲームの当たり判定がまだ無い
    if (player_) {
        CollisionManager::GetInstance()->DrawColliders();
    }
#endif
}

void GameDirector::UpdateCameraMode()
{
    if (player_->IsDead() || boss_->IsDead() || !isStart_) {
        return;
    }

    if (boss_->GetPhase() == 1) {
        cameraMode_ = false;
        // 移動制限を解除しステージ全体を移動可能にする
        player_->ClearDynamicBounds();
    }
    else if (boss_->GetPhase() == 2) {
        cameraMode_ = true;
        // ボス中心の戦闘エリアに移動制限
        Vector3 bossPos = boss_->GetTransform().translate;
        player_->SetDynamicBoundsFromCenter(bossPos, GameConst::BossPhase2AreaSize(), GameConst::BossPhase2AreaSize());
    }

    if (cameraMode_) {
        cameraManager_->ActivateController("ThirdPerson");
    }
    else {
        cameraManager_->ActivateController("TopDown");
    }

    player_->SetMode(cameraMode_);

}

void GameDirector::UpdateInput()
{
    // カメラアニメーション再生中は入力をリセット（デバッグビュー操作中の入力は Input 側で遮断される）
    if (animationController_->GetPlayState() == CameraAnimation::PlayState::PLAYING) {
        inputHandler_->ResetInputs();
    }
}

void GameDirector::InitializeObject3d()
{
    //-----------Player（Actor のスクリプト。自分の BeginPlay で初期化する）----------------//
    player_->SetCamera((*Renderer::GetInstance()->GetCamera()));
    player_->SetInputHandler(inputHandler_.get());

    //-----------Boss の初期化--------------------//
    boss_->SetPlayer(player_);
    // 開始演出が終わるまで一時停止
    boss_->SetIsPause(true);
    player_->SetBoss(boss_);

    controllerUI_->SetBoss(boss_);
}

void GameDirector::InitializeEmitterManager()
{
    // エミッターは、シーンの Effects の下の Actor（ParticleSystemComponent）が名前を付けて作ってある
    player_->SetForceFieldManager(forceFieldManager_);
    player_->SetEmitterManager(emitterManager_);
}

void GameDirector::InitializeEffectManager()
{
    overEffectManager_ = std::make_unique<OverEffectManager>(emitterManager_);
    overEffectManager_->SetTarget(player_);

    clearEffectManager_ = std::make_unique<ClearEffectManager>(emitterManager_);
    clearEffectManager_->SetTarget(boss_);

    bossBorderManager_ = std::make_unique<BossBorderParticleManager>(emitterManager_, GameConst::BossPhase2AreaSize());

    dashEffectManager_ = std::make_unique<DashEffectManager>(emitterManager_);
    dashEffectManager_->InitializePosition(player_->GetTranslate());
}

void GameDirector::CheckPause()
{
    // ゲーム開始後、演出中以外、生存中のみポーズ可能
    if (isStart_ && !player_->IsDead() && !boss_->IsDead() &&
        animationController_->GetPlayState() != CameraAnimation::PlayState::PLAYING) {
        if (inputHandler_->IsPaused()) {
            SetPaused(!isPaused_);
            if (isPaused_) {
                pauseMenu_->Reset();
            }
        }
    }
}

void GameDirector::UpdatePause()
{
    PauseMenu::Action action = pauseMenu_->Update();
    switch (action) {
    case PauseMenu::Action::Resume:
        SetPaused(false);
        break;
    case PauseMenu::Action::ToTitle:
        SceneManager::GetInstance()->ChangeScene(titleScene_, TransitionManager::EffectType::Fade, kFadeDuration);
        break;
    case PauseMenu::Action::ExitGame:
        // ×ボタンと同じ経路にして、Debug では未保存の変更の確認を通す
        WinApp::GetInstance()->RequestClose();
        break;
    default:
        break;
    }
}
void GameDirector::SetPaused(bool paused)
{
    isPaused_ = paused;
    GetOwner()->GetScene()->SetPaused(paused);
    controllerUI_->SetIsPaused(paused);
    pauseMenuLayout_->SetVisible(paused);
    player_->SetIsPause(paused);
    boss_->SetIsPause(paused);
    if (paused) {
        pauseBlurId_ = PostEffectManager::GetInstance()->AddEffect("GaussianBlur", GaussianBlurParam{ .sigma = 20.0f, .kernelSize = 30 });
    }
    else {
        PostEffectManager::GetInstance()->RemoveEffect(std::exchange(pauseBlurId_, 0));
    }
}
