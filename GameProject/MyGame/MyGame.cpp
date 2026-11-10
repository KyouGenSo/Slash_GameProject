#include "MyGame.h"

#include"Audio.h"
#include"Input.h"
#include "scene/SceneFactory.h"
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

    sceneFactory_ = std::make_unique<SceneFactory>();
    SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
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
