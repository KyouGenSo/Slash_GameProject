#include "MyGame.h"
#include"Audio.h"
#include"Input.h"
#include "scene/SceneFactory.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Draw2D.h"
#include "Object3dBasic.h"
#include "PostEffectManager.h"
#include "FrameTimer.h"
#include "GlobalVariables.h"
#include "ModelManager.h"
#include "GPUParticle.h"
#include "SpriteBasic.h"
#include "TransitionManager.h"

using namespace Tako;

void MyGame::Initialize()
{
    winApp_->SetWindowSize(1920, 1080);

    winApp_->SetWindowTitle(L"LE3B_12_キョウ_ゲンソ_Slash");

    TakoFramework::Initialize();

#pragma region 汎用機能初期化-------------------------------------------------------------------------------------------------------------------
    // 入力クラスの初期化
    Input::GetInstance()->Initialize(winApp_);

    // オーディオの初期化
    Audio::GetInstance()->Initialize("resources/Sound/");

#pragma endregion

    // シーンの初期化
    sceneFactory_ = std::make_unique<SceneFactory>();
    SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
    SceneManager::GetInstance()->ChangeScene("title", 0.0f);

    // テクスチャの読み込み
    LoadTextrue();

    // GlobalVariablesにパラメータを登録
    RegisterGlobalVariables();

    // GlobalVariablesのJsonファイル読み込み
    GlobalVariables::GetInstance()->LoadFiles();

    // SpriteBasicのリサイズコールバック関数登録
    spriteBasicOnresizeId_ = winApp_->RegisterOnResizeFunc(std::bind(&SpriteBasic::OnResize, SpriteBasic::GetInstance(), std::placeholders::_1));

    // GPUパーティクルの初期化
    GPUParticle::GetInstance()->Initialize(dx12_.get(), defaultCamera_.get());
}

void MyGame::Finalize()
{
    winApp_->UnregisterOnResizeFunc(spriteBasicOnresizeId_);

    // GPUパーティクルの解放
    GPUParticle::GetInstance()->Finalize();

    // Audioの解放
    Audio::GetInstance()->Finalize();

    // 入力クラスの解放
    Input::GetInstance()->Finalize();

    TakoFramework::Finalize();
}

void MyGame::Update()
{
    // カメラの更新
    defaultCamera_->Update();

    // 入力情報の更新
    Input::GetInstance()->Update();

    // F11キーでフルスクリーン切り替え
    if (Input::GetInstance()->TriggerKey(DIK_F11)) {
        ToggleFullScreen();
    }

    // GPUパーティクルの更新
    GPUParticle::GetInstance()->Update();

    TakoFramework::Update();

    //　サウンドの更新
    Audio::GetInstance()->Update();

    // ゲームパッドの状態をリフレッシュ
    Input::GetInstance()->RefreshGamePadState();
}

void MyGame::Draw()
{
    /// ============================================= ///
    /// ------------------シーン描画-------------------///
    /// ============================================= ///

    //ポストエフェクト適用対象のレンダーテクスチャを描画先に設定
    dx12_->SetEffectRenderTexture();

    // テクスチャ用のsrvヒープの設定
    SrvManager::GetInstance()->BeginDraw();

    SceneManager::GetInstance()->Draw();

    GPUParticle::GetInstance()->Draw();

    Draw2D::GetInstance()->Draw();

    /// ===================================================== ///
    /// ------------------ポストエフェクト描画-------------------///
    /// ===================================================== ///

      // ポストエフェクトの描画
    PostEffectManager::GetInstance()->Draw();

    /// ===================================================== ///
    /// ------------ポストエフェクト非適用対象の描画---------------///
    /// ===================================================== ///
    // ポストエフェクト非適用対象のレンダーテクスチャを描画先に設定
    dx12_->SetNonEffectRenderTexture();

    // シーンの描画
    SceneManager::GetInstance()->DrawWithoutEffect();

    TransitionManager::GetInstance()->Draw();

    Draw2D::GetInstance()->Reset();

    /// ============================================= ///
    /// ---------最終結果をスワップチェーンに描画---------///
    /// ============================================= ///
    bool isDrawToSwapChain = true;

#ifdef _DEBUG
    isDrawToSwapChain = !DebugUIManager::GetInstance()->IsWindowVisible("GameViewport");
#endif

    PostEffectManager::GetInstance()->DrawFinalResult(isDrawToSwapChain);


    /// ========================================= ///
    ///-------------------ImGui-------------------///
    /// ========================================= ///
#ifdef _DEBUG

    imguiManager_->Begin();

    TakoFramework::Draw();

    //SceneManager::GetInstance()->DrawImGui();

    Draw2D::GetInstance()->ImGui();

    imguiManager_->End();

    //imguiの描画
    imguiManager_->Draw();
#endif


    // 描画後の処理
    dx12_->EndDraw();
}

void MyGame::RegisterGlobalVariables()
{
    GlobalVariables* gv = GlobalVariables::GetInstance();

    // === Input === //
    gv->CreateGroup("Input");
    gv->AddItem("Input", "TriggerThreshold", 0.5f);

    // === GameScene === //
    gv->CreateGroup("GameScene");
    gv->AddItem("GameScene", "ShadowMaxDistance", 100.0f);
    gv->AddItem("GameScene", "DirectionalLightZ", -0.05f);
    // ダッシュエフェクトパラメータの登録
    gv->CreateGroup("DashEffect");
    gv->AddItem("DashEffect", "LerpSpeed", 35.0f);

    // === Player === //
    gv->CreateGroup("Player");
    gv->AddItem("Player", "BodyColliderSize", 3.2f);
    gv->AddItem("Player", "MeleeColliderX", 5.0f);
    gv->AddItem("Player", "MeleeColliderY", 2.0f);
    gv->AddItem("Player", "MeleeColliderZ", 17.0f);
    gv->AddItem("Player", "MeleeColliderOffsetZ", 10.0f);
    gv->AddItem("Player", "MoveInputDeadzone", 0.1f);
    gv->AddItem("Player", "RotationLerpSpeed", 0.2f);
    gv->AddItem("Player", "Speed", 0.5f);
    gv->AddItem("Player", "InitialY", 2.5f);
    gv->AddItem("Player", "InitialZ", -120.0f);
    gv->AddItem("Player", "AttackStartDistance", 5.0f);
    gv->AddItem("Player", "AttackMoveRotationLerp", 0.3f);
    gv->AddItem("Player", "BossLookatLerp", 1.15f);
    gv->AddItem("Player", "AttackMoveSpeed", 50.0f);

    // === MeleeAttack === //
    gv->CreateGroup("MeleeAttack");
    gv->AddItem("MeleeAttack", "AttackDamage", 10.0f);

    // === Boss === //
    gv->CreateGroup("Boss");
    gv->AddItem("Boss", "BodyColliderSize", 3.2f);
    gv->AddItem("Boss", "HitEffectDuration", 0.1f);
    gv->AddItem("Boss", "ShakeDuration", 0.3f);
    gv->AddItem("Boss", "ShakeIntensity", 0.2f);

    // === BossBullet === //
    gv->CreateGroup("BossBullet");
    gv->AddItem("BossBullet", "ColliderRadius", 1.0f);
    gv->AddItem("BossBullet", "Damage", 10.0f);
    gv->AddItem("BossBullet", "Lifetime", 5.0f);

    // === PenetratingBossBullet === //
    gv->CreateGroup("PenetratingBossBullet");
    gv->AddItem("PenetratingBossBullet", "ColliderRadius", 1.0f);
    gv->AddItem("PenetratingBossBullet", "Damage", 15.0f);
    gv->AddItem("PenetratingBossBullet", "Lifetime", 5.0f);

    // === CameraShake === //
    gv->CreateGroup("CameraShake");
    gv->AddItem("CameraShake", "Duration", 0.3f);
    gv->AddItem("CameraShake", "Intensity", 0.5f);

    // === PlayerBullet === //
    gv->CreateGroup("PlayerBullet");
    gv->AddItem("PlayerBullet", "Damage", 10.0f);
    gv->AddItem("PlayerBullet", "Lifetime", 3.0f);
    gv->AddItem("PlayerBullet", "ColliderRadius", 0.5f);
    gv->AddItem("PlayerBullet", "Speed", 30.0f);

    // === AttackState === //
    gv->CreateGroup("AttackState");
    gv->AddItem("AttackState", "SearchTime", 0.1f);
    gv->AddItem("AttackState", "MoveTime", 0.1f);
    gv->AddItem("AttackState", "AttackDuration", 0.1f);
    gv->AddItem("AttackState", "MaxCombo", 2);
    gv->AddItem("AttackState", "ComboWindow", 1.0f);
    gv->AddItem("AttackState", "BlockRadius", 4.0f);
    gv->AddItem("AttackState", "BlockScale", 0.5f);

    // === DashState === //
    gv->CreateGroup("DashState");
    gv->AddItem("DashState", "Duration", 0.05f);
    gv->AddItem("DashState", "Speed", 10.0f);
    gv->AddItem("DashState", "DashCooldown", 0.5f);

    // === ParryState === //
    gv->CreateGroup("ParryState");
    gv->AddItem("ParryState", "ParryDuration", 0.5f);
    gv->AddItem("ParryState", "ParrySuccessHealAmount", 5.0f);
    gv->AddItem("ParryState", "ParryCooldown", 1.0f);

    // === ShootState === //
    gv->CreateGroup("ShootState");
    gv->AddItem("ShootState", "FireRate", 0.2f);
    gv->AddItem("ShootState", "MoveSpeedMultiplier", 0.5f);
    gv->AddItem("ShootState", "AimRotationLerp", 0.3f);

    // === BossMeleeAttackCollider === //
    gv->CreateGroup("BossMeleeAttackCollider");
    gv->AddItem("BossMeleeAttackCollider", "Damage", 10.0f);
    gv->AddItem("BossMeleeAttackCollider", "ColliderSizeX", 2.0f);
    gv->AddItem("BossMeleeAttackCollider", "ColliderSizeY", 2.0f);
    gv->AddItem("BossMeleeAttackCollider", "ColliderSizeZ", 2.0f);
    gv->AddItem("BossMeleeAttackCollider", "OffsetZ", 3.0f);
}

void MyGame::LoadTextrue()
{
    TextureManager::GetInstance()->LoadTexture("white.png");
    TextureManager::GetInstance()->LoadTexture("black.png");
    TextureManager::GetInstance()->LoadTexture("circle.png");
    TextureManager::GetInstance()->LoadTexture("my_skybox.dds");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_1.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_2.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_3.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_4.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_5.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_6.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_7.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_8.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_9.png");
    TextureManager::GetInstance()->LoadTexture("title_text/title_text_10.png");
    TextureManager::GetInstance()->LoadTexture("title_button.png");
    TextureManager::GetInstance()->LoadTexture("game_button_text.png");
    TextureManager::GetInstance()->LoadTexture("gameClear_Text.png");
    TextureManager::GetInstance()->LoadTexture("gameOver_Text.png");

    TextureManager::GetInstance()->LoadTexture("button/A_Button_Down.png");
    TextureManager::GetInstance()->LoadTexture("button/A_Button_Up.png");
    TextureManager::GetInstance()->LoadTexture("button/B_Button_Down.png");
    TextureManager::GetInstance()->LoadTexture("button/B_Button_Up.png");
    TextureManager::GetInstance()->LoadTexture("button/X_Button_Down.png");
    TextureManager::GetInstance()->LoadTexture("button/X_Button_Up.png");
    TextureManager::GetInstance()->LoadTexture("button/Y_Button_Down.png");
    TextureManager::GetInstance()->LoadTexture("button/Y_Button_Up.png");

    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_01.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_02.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_03.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_04.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_05.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_06.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_07.png");
    TextureManager::GetInstance()->LoadTexture("joystick/L_Joystick_08.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_01.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_02.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_03.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_04.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_05.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_06.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_07.png");
    TextureManager::GetInstance()->LoadTexture("joystick/R_Joystick_08.png");

    TextureManager::GetInstance()->LoadTexture("kougeki.png");
    TextureManager::GetInstance()->LoadTexture("dash.png");
    TextureManager::GetInstance()->LoadTexture("parry.png");
    TextureManager::GetInstance()->LoadTexture("shageki.png");
    TextureManager::GetInstance()->LoadTexture("idou.png");
}
