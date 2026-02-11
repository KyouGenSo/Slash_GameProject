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

using namespace Tako;

void ClearScene::Initialize()
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
    (*Object3dBasic::GetInstance()->GetCamera())->SetTranslate(Vector3(0.0f, cameraHiddenY_, -34.0f));

    // spriteの初期化
    backGround_ = std::make_unique<Sprite>();
    backGround_->Initialize("black.dds");
    backGround_->SetPos(Vector2(0.f, 0.f));
    backGround_->SetSize(Vector2(static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight)));

    titleText_ = std::make_unique<Sprite>();
    titleText_->Initialize("gameClear_Text.dds");
    titleText_->SetPos(Vector2(WinApp::clientWidth / 2.f - titleTextHalfWidth_, titleTextY_));

    pressButtonText_ = std::make_unique<Sprite>();
    pressButtonText_->Initialize("titlescene_button.dds");
    pressButtonText_->SetPos(Vector2(
        WinApp::clientWidth / 2.f - pressButtonText_->GetSize().x / 2.f,
        WinApp::clientHeight - buttonBottomOffset_));

}


void ClearScene::Finalize()
{

}

void ClearScene::Update()
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

void ClearScene::Draw()
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

void ClearScene::DrawWithoutEffect()
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

void ClearScene::DrawImGui()
{
#ifdef _DEBUG

    /// ================================== ///
    ///             ImGuiの描画              ///
    /// ================================== ///


#endif // _DEBUG
}