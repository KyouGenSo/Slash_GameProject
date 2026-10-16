#include "ResultScene.h"
#include "LineRenderer.h"
#include "GPUParticle.h"
#include "Input.h"
#include "Object3dBasic.h"
#include "SceneManager.h"
#include "SpriteBasic.h"

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

    (*Object3dBasic::GetInstance()->GetCamera())->SetRotate(Vector3(0.2f, 0.0f, 0.0f));
    (*Object3dBasic::GetInstance()->GetCamera())->SetTranslate(Vector3(0.0f, cameraHiddenY_, -34.0f));

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
    //------------------背景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    layout_.Draw("Background");


    //-------------------Model の描画-------------------//
    Object3dBasic::GetInstance()->SetCommonRenderSetting();




    //------------------前景 Sprite の描画------------------//
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    layout_.Draw("Title");

    layout_.Draw("PressButton");

}

void ResultScene::DrawWithoutEffect()
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




}

void ResultScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGui の描画              ///
    /// ================================== ///


#endif // _DEBUG
}
