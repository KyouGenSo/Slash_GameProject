#include "ControllerUI.h"
#include "Actor.h"
#include "Input.h"
#include "Logger.h"
#include "Object/Boss/Boss.h"
#include "UILayoutComponent.h"
#include <cmath>
#include <DirectXMath.h>
#include <format>

#ifdef _DEBUG
#include "imgui.h"
#endif

using namespace Tako;

void ControllerUI::BeginPlay()
{
    UILayoutComponent* component = GetOwner()->GetComponent<UILayoutComponent>();
    if (!component) {
        Logger::Log("ControllerUI: the Actor has no UILayoutComponent\n");
        return;
    }
    layout_ = &component->GetLayout();
    SetIsPaused(false);
}

void ControllerUI::Tick([[maybe_unused]] float deltaTime)
{
    if (!layout_) {
        return;
    }
    Input* input = Input::GetInstance();

    isAPressed_ = input->PushButton(GamepadButton::A);
    isBPressed_ = input->PushButton(GamepadButton::B);
    isXPressed_ = input->PushButton(GamepadButton::X);
    isYPressed_ = input->PushButton(GamepadButton::Y);

    leftStickDir_ = GetStickDirectionIndex(input->GetLeftStick());
    rightStickDir_ = GetStickDirectionIndex(input->GetRightStick());

    // 押下中のボタンは Down の画像に差し替える
    layout_->SetVisible("AButtonUp", !isAPressed_);
    layout_->SetVisible("AButtonDown", isAPressed_);
    layout_->SetVisible("BButtonUp", !isBPressed_);
    layout_->SetVisible("BButtonDown", isBPressed_);
    layout_->SetVisible("XButtonUp", !isXPressed_);
    layout_->SetVisible("XButtonDown", isXPressed_);
    layout_->SetVisible("YButtonUp", !isYPressed_);
    layout_->SetVisible("YButtonDown", isYPressed_);

    // スティックは向いている方向の 1 枚だけを表示する
    for (int i = 0; i < kStickDirectionCount; ++i) {
        layout_->SetVisible(std::format("LJoystick{}", i + 1), i == leftStickDir_);
        layout_->SetVisible(std::format("RJoystick{}", i + 1), i == rightStickDir_);
    }

    // 右ジョイスティックと射撃はフェーズ2では非表示
    const bool isPhase2 = boss_ && boss_->GetPhase() == 2;
    layout_->SetVisible("RightStick", !isPhase2);
    layout_->SetVisible("Shageki", !isPhase2);

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

void ControllerUI::SetIsPaused(bool isPaused)
{
    isPaused_ = isPaused;
    if (!layout_) {
        return;
    }
    // ポーズ中の操作説明は PauseMenu 側が描画する
    layout_->SetVisible("Controls", !isPaused_);
}

#ifdef _DEBUG
bool ControllerUI::DrawCustomImGui()
{
    ImGui::SeparatorText("Button States");
    ImGui::Text("A: %s", isAPressed_ ? "Pressed" : "Released");
    ImGui::Text("B: %s", isBPressed_ ? "Pressed" : "Released");
    ImGui::Text("X: %s", isXPressed_ ? "Pressed" : "Released");
    ImGui::Text("Y: %s", isYPressed_ ? "Pressed" : "Released");

    ImGui::SeparatorText("Stick Directions");
    // GetStickDirectionIndex の並び（上から時計回り）
    const char* dirNames[kStickDirectionCount] = { "Up", "UpRight", "Right", "DownRight", "Down", "DownLeft", "Left", "UpLeft" };
    ImGui::Text("Left Stick: %s (index: %d)", dirNames[leftStickDir_], leftStickDir_);
    ImGui::Text("Right Stick: %s (index: %d)", dirNames[rightStickDir_], rightStickDir_);
    return false;
}
#endif
