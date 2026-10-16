#include "ControllerUI.h"
#include "Input.h"
#include "Object/Boss/Boss.h"
#include <cmath>
#include <DirectXMath.h>
#include <format>

#ifdef _DEBUG
#include "imgui.h"
#endif

using namespace Tako;

void ControllerUI::Initialize()
{
    layout_.Load("ControllerUI");
    SetIsPaused(false);
}

void ControllerUI::Update()
{
    Input* input = Input::GetInstance();

    isAPressed_ = input->PushButton(GamepadButton::A);
    isBPressed_ = input->PushButton(GamepadButton::B);
    isXPressed_ = input->PushButton(GamepadButton::X);
    isYPressed_ = input->PushButton(GamepadButton::Y);

    leftStickDir_ = GetStickDirectionIndex(input->GetLeftStick());
    rightStickDir_ = GetStickDirectionIndex(input->GetRightStick());

    // 押下中のボタンは Down の画像に差し替える
    layout_.SetVisible("AButtonUp", !isAPressed_);
    layout_.SetVisible("AButtonDown", isAPressed_);
    layout_.SetVisible("BButtonUp", !isBPressed_);
    layout_.SetVisible("BButtonDown", isBPressed_);
    layout_.SetVisible("XButtonUp", !isXPressed_);
    layout_.SetVisible("XButtonDown", isXPressed_);
    layout_.SetVisible("YButtonUp", !isYPressed_);
    layout_.SetVisible("YButtonDown", isYPressed_);

    // スティックは向いている方向の 1 枚だけを表示する
    for (int i = 0; i < kStickDirectionCount; ++i) {
        layout_.SetVisible(std::format("LJoystick{}", i + 1), i == leftStickDir_);
        layout_.SetVisible(std::format("RJoystick{}", i + 1), i == rightStickDir_);
    }

    // 右ジョイスティックと射撃はフェーズ2では非表示
    const bool isPhase2 = boss_ && boss_->GetPhase() == 2;
    layout_.SetVisible("RightStick", !isPhase2);
    layout_.SetVisible("Shageki", !isPhase2);

    layout_.Update();
}

int ControllerUI::GetStickDirectionIndex(const Vector2& stick) const
{
    float magnitude = std::sqrt(stick.x * stick.x + stick.y * stick.y);

    // デッドゾーン未満は上(0)を返す
    if (magnitude < stickDeadzone_) {
        return 0;
    }

    // atan2(x, y) で「上」を0度基準にする
    float angle = DirectX::XMConvertToDegrees(std::atan2(stick.x, stick.y));

    // 境界を方向の「間」に置くため 22.5度オフセット
    angle += 22.5f;
    if (angle < 0.0f) {
        angle += 360.0f;
    }

    // 45度ずつ8方向に分類
    int index = static_cast<int>(angle / 45.0f) % kStickDirectionCount;
    return index;
}

void ControllerUI::Draw()
{
    layout_.Draw();
}

void ControllerUI::SetIsPaused(bool isPaused)
{
    isPaused_ = isPaused;
    // ポーズ中の操作説明は PauseMenu 側が描画する
    layout_.SetVisible("Controls", !isPaused_);
}

void ControllerUI::DrawImGui()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("ControllerUI")) {
        ImGui::Text("Button States:");
        ImGui::Text("  A: %s", isAPressed_ ? "Pressed" : "Released");
        ImGui::Text("  B: %s", isBPressed_ ? "Pressed" : "Released");
        ImGui::Text("  X: %s", isXPressed_ ? "Pressed" : "Released");
        ImGui::Text("  Y: %s", isYPressed_ ? "Pressed" : "Released");

        ImGui::Separator();
        ImGui::Text("Stick Directions:");
        // GetStickDirectionIndex の並び（上から時計回り）
        const char* dirNames[kStickDirectionCount] = { "Up", "UpRight", "Right", "DownRight", "Down", "DownLeft", "Left", "UpLeft" };
        ImGui::Text("  Left Stick: %s (index: %d)", dirNames[leftStickDir_], leftStickDir_);
        ImGui::Text("  Right Stick: %s (index: %d)", dirNames[rightStickDir_], rightStickDir_);

        ImGui::Separator();
        ImGui::SliderFloat("Deadzone", &stickDeadzone_, 0.0f, 1.0f);
        ImGui::TextDisabled("Layout \"%s\" is edited in Tools > UI Editor", layout_.GetName().c_str());

        ImGui::TreePop();
    }
#endif
}
