#include "OverScene.h"
#include "ClearScene.h"

#include "Draw2D.h"
#include "GPUParticle.h"
#include "Input.h"
#include "Object3dBasic.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "SpriteBasic.h"

#ifdef _DEBUG
#include"ImGui.h"
#include "DebugCamera.h"
#endif

void OverScene::Initialize()
{
#ifdef _DEBUG
    DebugCamera::GetInstance()->Initialize();
    Object3dBasic::GetInstance()->SetDebug(false);
    Draw2D::GetInstance()->SetDebug(false);
    GPUParticle::GetInstance()->SetIsDebug(false);
#endif

    /// ================================== ///
    ///              初期化処理              ///
    /// ================================== ///

    // カメラの回転と位置を設定
    (*Object3dBasic::GetInstance()->GetCamera())->SetRotate(Vector3(0.2f, 0.0f, 0.0f));
    (*Object3dBasic::GetInstance()->GetCamera())->SetTranslate(Vector3(0.0f, -1000.0f, -34.0f));

    // spriteの初期化
    backGround_ = std::make_unique<Sprite>();
    backGround_->Initialize("black.png");
    backGround_->SetPos(Vector2(0.f, 0.f));
    backGround_->SetSize(Vector2(static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight)));

    titleText_ = std::make_unique<Sprite>();
    titleText_->Initialize("gameOver_Text.png");
    titleText_->SetPos(Vector2(WinApp::clientWidth / 2.f - 250.f, 300.f));

    pressButtonText_ = std::make_unique<Sprite>();
    pressButtonText_->Initialize("title_button.png");
    pressButtonText_->SetPos(Vector2(
        WinApp::clientWidth / 2.f - pressButtonText_->GetSize().x / 2.f,
        WinApp::clientHeight - 300.f));

}


void OverScene::Finalize()
{

}

void OverScene::Update()
{
    /// ================================== ///
    ///              更新処理               ///
    /// ================================== ///

    backGround_->Update();
    titleText_->Update();
    pressButtonText_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneManager::GetInstance()->ChangeScene("title");
    }

    if (Input::GetInstance()->TriggerButton(XButtons.A)) {
        SceneManager::GetInstance()->ChangeScene("title");
    }
}

void OverScene::Draw()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///
    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    // 背景スプライト描画
    backGround_->Draw();


    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();




    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();

    // タイトルスプライト描画
    titleText_->Draw();

    // プレスボタンテキストスプライト描画
    pressButtonText_->Draw();

}

void OverScene::DrawWithoutEffect()
{
    /// ================================== ///
    ///              描画処理               ///
    /// ================================== ///

    //------------------背景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();




    //-------------------Modelの描画-------------------//
    // 3Dモデル共通描画設定
    Object3dBasic::GetInstance()->SetCommonRenderSetting();





    //------------------前景Spriteの描画------------------//
    // スプライト共通描画設定
    SpriteBasic::GetInstance()->SetCommonRenderSetting();




}

void OverScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///


#endif // _DEBUG
}