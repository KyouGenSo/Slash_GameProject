#include "MyGame.h"

#include"Audio.h"
#include"Input.h"
#include "scene/SceneFactory.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "LineRenderer.h"
#include "PostEffectManager.h"
#include "FrameTimer.h"
#include "ModelManager.h"
#include "GPUParticle.h"
#include "TransitionManager.h"


#ifdef _DEBUG
#include "DebugUIManager.h"
#endif

using namespace Tako;

void MyGame::Initialize()
{
    TakoFramework::Initialize();

    sceneFactory_ = std::make_unique<SceneFactory>();
    SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
    SceneManager::GetInstance()->ChangeToStartupScene();

    LoadTextures();
}

void MyGame::Finalize()
{
    TakoFramework::Finalize();
}

void MyGame::Update()
{
    // F11キーでフルスクリーン切り替え
    if (Input::GetInstance()->TriggerKey(DIK_F11)) {
        ToggleFullScreen();
    }

    TakoFramework::Update();

}

void MyGame::Draw()
{
    TakoFramework::Draw();
}

void MyGame::LoadTextures()
{
    LoadTitleTextures();
    LoadButtonTextures();
    LoadJoystickTextures();
    LoadActionIconTextures();
}

void MyGame::LoadTitleTextures()
{
    TextureManager* tm = TextureManager::GetInstance();

    // タイトルテキストアニメーション用（10 コマを 5 列×2 行に並べたシート）
    tm->LoadTexture("title_text/title_text_sheet.png");

    // シーン遷移用テキスト
    tm->LoadTexture("titlescene_button.dds");
    tm->LoadTexture("game_button_text.dds");
    tm->LoadTexture("gameClear_Text.dds");
    tm->LoadTexture("gameOver_Text.dds");

    // ポーズメニュー用
    tm->LoadTexture("PauseButton_Resume.dds");
    tm->LoadTexture("PauseButton_Title.dds");
    tm->LoadTexture("PauseButton_Exit.dds");
    tm->LoadTexture("PauseMenu_Text.dds");
}

void MyGame::LoadButtonTextures()
{
    TextureManager* tm = TextureManager::GetInstance();
    tm->LoadTexture("button/A_Button_Down.dds");
    tm->LoadTexture("button/A_Button_Up.dds");
    tm->LoadTexture("button/B_Button_Down.dds");
    tm->LoadTexture("button/B_Button_Up.dds");
    tm->LoadTexture("button/X_Button_Down.dds");
    tm->LoadTexture("button/X_Button_Up.dds");
    tm->LoadTexture("button/Y_Button_Down.dds");
    tm->LoadTexture("button/Y_Button_Up.dds");
    tm->LoadTexture("button/Menu_Button_Up.dds");
    tm->LoadTexture("button/DPAD_Neutral.dds");
    tm->LoadTexture("button/DPAD_Up.dds");
    tm->LoadTexture("button/DPAD_Down.dds");
}

void MyGame::LoadJoystickTextures()
{
    TextureManager* tm = TextureManager::GetInstance();

    // 左スティック
    tm->LoadTexture("joystick/L_Joystick_01.dds");
    tm->LoadTexture("joystick/L_Joystick_02.dds");
    tm->LoadTexture("joystick/L_Joystick_03.dds");
    tm->LoadTexture("joystick/L_Joystick_04.dds");
    tm->LoadTexture("joystick/L_Joystick_05.dds");
    tm->LoadTexture("joystick/L_Joystick_06.dds");
    tm->LoadTexture("joystick/L_Joystick_07.dds");
    tm->LoadTexture("joystick/L_Joystick_08.dds");

    // 右スティック
    tm->LoadTexture("joystick/R_Joystick_01.dds");
    tm->LoadTexture("joystick/R_Joystick_02.dds");
    tm->LoadTexture("joystick/R_Joystick_03.dds");
    tm->LoadTexture("joystick/R_Joystick_04.dds");
    tm->LoadTexture("joystick/R_Joystick_05.dds");
    tm->LoadTexture("joystick/R_Joystick_06.dds");
    tm->LoadTexture("joystick/R_Joystick_07.dds");
    tm->LoadTexture("joystick/R_Joystick_08.dds");
}

void MyGame::LoadActionIconTextures()
{
    TextureManager* tm = TextureManager::GetInstance();
    tm->LoadTexture("kougeki.dds");
    tm->LoadTexture("dash.dds");
    tm->LoadTexture("parry.dds");
    tm->LoadTexture("shageki.dds");
    tm->LoadTexture("idou.dds");
    tm->LoadTexture("kettei.dds");
    tm->LoadTexture("sentaku.dds");
}
