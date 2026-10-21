#include "ResultScene.h"
#include "LineRenderer.h"
#include "GPUParticle.h"
#include "Input.h"
#include "Renderer.h"
#include "SceneManager.h"

#ifdef _DEBUG
#include"ImGui.h"
#endif

using namespace Tako;

ResultScene::ResultScene(const std::string& titleTexture)
    : titleTexture_(titleTexture)
{
}

void ResultScene::Initialize()
{
    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///

    (*Renderer::GetInstance()->GetCamera())->SetRotate(Vector3(0.2f, 0.0f, 0.0f));
    (*Renderer::GetInstance()->GetCamera())->SetTranslate(Vector3(0.0f, cameraHiddenY_, -34.0f));

    layout_.Load("Result");
    // クリアとゲームオーバーで配置は共通。タイトルの画像だけをコンストラクタで受け取ったものに差し替える
    layout_.SetTexture("Title", titleTexture_);

}


void ResultScene::Finalize()
{

}

void ResultScene::Update()
{
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    layout_.Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }

    if (Input::GetInstance()->TriggerButton(GamepadButton::A)) {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void ResultScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    layout_.Draw("Background");

    layout_.Draw("Title");

    layout_.Draw("PressButton");
}

void ResultScene::DrawWithoutEffect()
{
}

void ResultScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGui の描画              ///
    /// ================================== ///


#endif // _DEBUG
}
