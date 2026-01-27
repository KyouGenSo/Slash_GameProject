#include "PauseMenu.h"
#include "Sprite.h"
#include "Input.h"

using namespace Tako;

void PauseMenu::Initialize()
{
    // 半透明黒背景オーバーレイ
    overlaySprite_ = std::make_unique<Sprite>();
    overlaySprite_->Initialize("white.png");
    overlaySprite_->SetPos({ 0.0f, 0.0f });
    overlaySprite_->SetSize({ 1920.0f, 1080.0f });
    overlaySprite_->SetColor({ 0.0f, 0.0f, 0.0f, 0.5f });

    // タイトルスプライト（PAUSE表示）
    titleSprite_ = std::make_unique<Sprite>();
    titleSprite_->Initialize("PauseMenu_Text.png");
    titleSprite_->SetPos({ 960.0f, 200.0f });
    titleSprite_->SetSize({ 400.0f, 100.0f });
    titleSprite_->SetAnchorPoint({ 0.5f, 0.5f });

    // ボタンスプライト初期化
    const char* buttonTextures[kButtonCount] = {
        "PauseButton_Resume.png",
        "PauseButton_Title.png",
        "PauseButton_Exit.png"
    };

    const float buttonY[kButtonCount] = { 400.0f, 550.0f, 700.0f };

    for (int i = 0; i < kButtonCount; ++i) {
        buttonSprites_[i] = std::make_unique<Sprite>();
        buttonSprites_[i]->Initialize(buttonTextures[i]);
        buttonSprites_[i]->SetPos({ 960.0f, buttonY[i] });
        buttonSprites_[i]->SetSize({ 300.0f, 100.0f });
        buttonSprites_[i]->SetAnchorPoint({ 0.5f, 0.5f });
    }

    // DPAD操作ガイドスプライト（ControllerUIと同じ座標・サイズ）
    dpadGuideSprite_ = std::make_unique<Sprite>();
    dpadGuideSprite_->Initialize("button/DPAD_Neutral.png");
    dpadGuideSprite_->SetPos({ 323.0f, 869.0f });
    dpadGuideSprite_->SetSize({ 150.0f, 150.0f });

    dpadUpSprite_ = std::make_unique<Sprite>();
    dpadUpSprite_->Initialize("button/DPAD_Up.png");
    dpadUpSprite_->SetPos({ 323.0f, 869.0f });
    dpadUpSprite_->SetSize({ 150.0f, 150.0f });

    dpadDownSprite_ = std::make_unique<Sprite>();
    dpadDownSprite_->Initialize("button/DPAD_Down.png");
    dpadDownSprite_->SetPos({ 323.0f, 869.0f });
    dpadDownSprite_->SetSize({ 150.0f, 150.0f });

    // Aボタンスプライト（ControllerUIと同じ座標・サイズ）
    aButtonUpSprite_ = std::make_unique<Sprite>();
    aButtonUpSprite_->Initialize("button/A_Button_Up.png");
    aButtonUpSprite_->SetPos({ 1565.0f, 948.0f });
    aButtonUpSprite_->SetSize({ 60.0f, 60.0f });

    aButtonDownSprite_ = std::make_unique<Sprite>();
    aButtonDownSprite_->Initialize("button/A_Button_Down.png");
    aButtonDownSprite_->SetPos({ 1565.0f, 948.0f });
    aButtonDownSprite_->SetSize({ 60.0f, 60.0f });

    // 選択テキスト（ControllerUIと同じ座標・サイズ）
    sentakuSprite_ = std::make_unique<Sprite>();
    sentakuSprite_->Initialize("sentaku.png");
    sentakuSprite_->SetPos({ 450.0f, 892.0f });
    sentakuSprite_->SetSize({ 150.0f, 50.0f });

    // 決定テキスト（ControllerUIと同じ座標・サイズ）
    ketteiSprite_ = std::make_unique<Sprite>();
    ketteiSprite_->Initialize("kettei.png");
    ketteiSprite_->SetPos({ 1518.0f, 997.0f });
    ketteiSprite_->SetSize({ 150.0f, 50.0f });

    // 初期選択状態を設定
    Reset();
}

PauseMenu::Action PauseMenu::Update()
{
    Input* input = Input::GetInstance();

    // 入力状態を取得（描画用）
    isDPadUpPressed_ = input->PushButton(XButtons.DPad_Up);
    isDPadDownPressed_ = input->PushButton(XButtons.DPad_Down);
    isAPressed_ = input->PushButton(XButtons.A);

    // DPAD上下で選択移動
    if (input->TriggerButton(XButtons.DPad_Up)) {
        selectedIndex_ = (selectedIndex_ - 1 + kButtonCount) % kButtonCount;
        UpdateButtonColors();
    }
    if (input->TriggerButton(XButtons.DPad_Down)) {
        selectedIndex_ = (selectedIndex_ + 1) % kButtonCount;
        UpdateButtonColors();
    }

    // Aボタンで決定
    if (input->TriggerButton(XButtons.A)) {
        switch (selectedIndex_) {
        case 0: return Action::Resume;
        case 1: return Action::ToTitle;
        case 2: return Action::ExitGame;
        }
    }

    // スプライト更新
    overlaySprite_->Update();
    titleSprite_->Update();
    for (auto& button : buttonSprites_) {
        button->Update();
    }
    dpadGuideSprite_->Update();
    dpadUpSprite_->Update();
    dpadDownSprite_->Update();
    aButtonUpSprite_->Update();
    aButtonDownSprite_->Update();
    ketteiSprite_->Update();
    sentakuSprite_->Update();

    return Action::None;
}

void PauseMenu::Draw()
{
    // 背景オーバーレイ
    overlaySprite_->Draw();

    // タイトル
    titleSprite_->Draw();

    // ボタン
    for (auto& button : buttonSprites_) {
        button->Draw();
    }

    // === 操作説明UI ===

    // 選択テキスト
    sentakuSprite_->Draw();

    // DPAD（入力に応じて切り替え）
    if (isDPadUpPressed_) {
        dpadUpSprite_->Draw();
    } else if (isDPadDownPressed_) {
        dpadDownSprite_->Draw();
    } else {
        dpadGuideSprite_->Draw();
    }

    // 決定テキスト
    ketteiSprite_->Draw();

    // Aボタン（入力に応じて切り替え）
    if (isAPressed_) {
        aButtonDownSprite_->Draw();
    } else {
        aButtonUpSprite_->Draw();
    }
}

void PauseMenu::Reset()
{
    selectedIndex_ = 0;
    UpdateButtonColors();
}

void PauseMenu::UpdateButtonColors()
{
    for (int i = 0; i < kButtonCount; ++i) {
        if (i == selectedIndex_) {
            // 選択中は白
            buttonSprites_[i]->SetColor({ kSelectedColorR, kSelectedColorG, kSelectedColorB, 1.0f });
        } else {
            // 非選択はグレー
            buttonSprites_[i]->SetColor({ kUnselectedColorR, kUnselectedColorG, kUnselectedColorB, 1.0f });
        }
    }
}
