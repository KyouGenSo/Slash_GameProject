#include "PauseMenu.h"
#include "Input.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

using namespace Tako;

void PauseMenu::Initialize()
{
    layout_.Load("PauseMenu");
    Reset();
}

PauseMenu::Action PauseMenu::Update()
{
    Input* input = Input::GetInstance();

    isDPadUpPressed_ = input->PushButton(GamepadButton::DPad_Up);
    isDPadDownPressed_ = input->PushButton(GamepadButton::DPad_Down);
    isAPressed_ = input->PushButton(GamepadButton::A);

    // DPAD 上下で選択移動
    if (input->TriggerButton(GamepadButton::DPad_Up)) {
        selectedIndex_ = (selectedIndex_ - 1 + kButtonCount) % kButtonCount;
    }
    if (input->TriggerButton(GamepadButton::DPad_Down)) {
        selectedIndex_ = (selectedIndex_ + 1) % kButtonCount;
    }

    // 押下中のボタン画像に差し替える（上下同時押しは上を優先）
    layout_.SetVisible("DpadNeutral", !isDPadUpPressed_ && !isDPadDownPressed_);
    layout_.SetVisible("DpadUp", isDPadUpPressed_);
    layout_.SetVisible("DpadDown", !isDPadUpPressed_ && isDPadDownPressed_);
    layout_.SetVisible("AButtonUp", !isAPressed_);
    layout_.SetVisible("AButtonDown", isAPressed_);

    // エディタで Reload されても選択色が戻るよう毎フレーム設定する
    UpdateButtonColors();
    layout_.Update();

    if (input->TriggerButton(GamepadButton::A)) {
        switch (selectedIndex_) {
        case 0: return Action::Resume;
        case 1: return Action::ToTitle;
        case 2: return Action::ExitGame;
        }
    }

    return Action::None;
}

void PauseMenu::Draw()
{
    layout_.Draw();
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
            layout_.SetColor(kButtonNames[i], { kSelectedColorR, kSelectedColorG, kSelectedColorB, 1.0f });
        } else {
            layout_.SetColor(kButtonNames[i], { kUnselectedColorR, kUnselectedColorG, kUnselectedColorB, 1.0f });
        }
    }
}

void PauseMenu::DrawImGui()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("PauseMenu")) {
        ImGui::Text("Selected Index: %d", selectedIndex_);
        ImGui::Text("Input States:");
        ImGui::Text("  DPad Up: %s", isDPadUpPressed_ ? "Pressed" : "Released");
        ImGui::Text("  DPad Down: %s", isDPadDownPressed_ ? "Pressed" : "Released");
        ImGui::Text("  A Button: %s", isAPressed_ ? "Pressed" : "Released");
        ImGui::TextDisabled("Layout \"%s\" is edited in Tools > UI Editor", layout_.GetName().c_str());
        ImGui::TreePop();
    }
#endif
}
