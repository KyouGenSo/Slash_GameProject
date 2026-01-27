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

    // DPAD操作ガイドスプライト
    dpadGuideSprite_ = std::make_unique<Sprite>();
    dpadGuideSprite_->Initialize("button/DPAD_Neutral_Xbox.png");
    dpadGuideSprite_->SetPos({ 960.0f, 900.0f });
    dpadGuideSprite_->SetSize({ 100.0f, 100.0f });
    dpadGuideSprite_->SetAnchorPoint({ 0.5f, 0.5f });

    // 初期選択状態を設定
    Reset();
}

PauseMenu::Action PauseMenu::Update()
{
    Input* input = Input::GetInstance();

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

    // DPAD操作ガイド
    dpadGuideSprite_->Draw();
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
