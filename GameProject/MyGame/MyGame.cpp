#include "MyGame.h"

#include"Audio.h"
#include"Input.h"
#include "SceneManager.h"
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

    // シーンはすべて resources/Json/Scenes のファイルから作る（C++ のシーンクラスは無い）
    SceneManager::GetInstance()->ChangeToStartupScene();
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
