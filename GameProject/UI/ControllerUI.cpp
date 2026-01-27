#include "ControllerUI.h"
#include "WinApp.h"
#include "Input.h"
#include "Object/Boss/Boss.h"
#include <cmath>
#include <string>
#include <functional>

#ifdef _DEBUG
#include "imgui.h"
#endif

using namespace Tako;

ControllerUI::~ControllerUI()
{
    // リサイズコールバックを解除（ダングリングポインタ防止）
    if (winApp_ && onResizeId_ != 0) {
        winApp_->UnregisterOnResizeFunc(onResizeId_);
    }
}

void ControllerUI::Initialize()
{
    // ボタンスプライト初期化（Up状態）
    aButtonUpSprite_ = std::make_unique<Sprite>();
    aButtonUpSprite_->Initialize("button/A_Button_Up.png");
    aButtonUpSprite_->SetPos({ 1565.0f, 948.0f });
    aButtonUpSprite_->SetSize({ 60.0f, 60.0f });

    aButtonDownSprite_ = std::make_unique<Sprite>();
    aButtonDownSprite_->Initialize("button/A_Button_Down.png");
    aButtonDownSprite_->SetPos({ 1565.0f, 948.0f });
    aButtonDownSprite_->SetSize({ 60.0f, 60.0f });

    bButtonUpSprite_ = std::make_unique<Sprite>();
    bButtonUpSprite_->Initialize("button/B_Button_Up.png");
    bButtonUpSprite_->SetPos({ 1640.0f, 869.0f });
    bButtonUpSprite_->SetSize({ 60.0f, 60.0f });

    bButtonDownSprite_ = std::make_unique<Sprite>();
    bButtonDownSprite_->Initialize("button/B_Button_Down.png");
    bButtonDownSprite_->SetPos({ 1640.0f, 869.0f });
    bButtonDownSprite_->SetSize({ 60.0f, 60.0f });

    xButtonUpSprite_ = std::make_unique<Sprite>();
    xButtonUpSprite_->Initialize("button/X_Button_Up.png");
    xButtonUpSprite_->SetPos({ 1488.0f, 869.0f });
    xButtonUpSprite_->SetSize({ 60.0f, 60.0f });

    xButtonDownSprite_ = std::make_unique<Sprite>();
    xButtonDownSprite_->Initialize("button/X_Button_Down.png");
    xButtonDownSprite_->SetPos({ 1488.0f, 869.0f });
    xButtonDownSprite_->SetSize({ 60.0f, 60.0f });

    yButtonUpSprite_ = std::make_unique<Sprite>();
    yButtonUpSprite_->Initialize("button/Y_Button_Up.png");
    yButtonUpSprite_->SetPos({ 1565.0f, 803.0f });
    yButtonUpSprite_->SetSize({ 60.0f, 60.0f });

    yButtonDownSprite_ = std::make_unique<Sprite>();
    yButtonDownSprite_->Initialize("button/Y_Button_Down.png");
    yButtonDownSprite_->SetPos({ 1565.0f, 803.0f });
    yButtonDownSprite_->SetSize({ 60.0f, 60.0f });

    // ジョイスティックスプライト初期化（8方向）
    for (int i = 0; i < 8; ++i) {
        lJoystickSprites_[i] = std::make_unique<Sprite>();
        lJoystickSprites_[i]->Initialize("joystick/L_Joystick_0" + std::to_string(i + 1) + ".png");
        lJoystickSprites_[i]->SetPos({ 323.0f, 869.0f });
        lJoystickSprites_[i]->SetSize({ 150.0f, 150.0f });

        rJoystickSprites_[i] = std::make_unique<Sprite>();
        rJoystickSprites_[i]->Initialize("joystick/R_Joystick_0" + std::to_string(i + 1) + ".png");
        rJoystickSprites_[i]->SetPos({ 791.0f, 869.0f });
        rJoystickSprites_[i]->SetSize({ 150.0f, 150.0f });
    }

    // アクションアイコン初期化
    kougekiSprite_ = std::make_unique<Sprite>();
    kougekiSprite_->Initialize("kougeki.png");
    kougekiSprite_->SetPos({ 1349.0f, 871.0f });
    kougekiSprite_->SetSize({ 150.0f, 50.0f });

    dashSprite_ = std::make_unique<Sprite>();
    dashSprite_->Initialize("dash.png");
    dashSprite_->SetPos({ 1518.0f, 997.0f });
    dashSprite_->SetSize({ 150.0f, 50.0f });

    parrySprite_ = std::make_unique<Sprite>();
    parrySprite_->Initialize("parry.png");
    parrySprite_->SetPos({ 1701.0f, 871.0f });
    parrySprite_->SetSize({ 150.0f, 50.0f });

    shagekiSprite_ = std::make_unique<Sprite>();
    shagekiSprite_->Initialize("shageki.png");
    shagekiSprite_->SetPos({ 913.0f, 892.0f });
    shagekiSprite_->SetSize({ 150.0f, 50.0f });

    idouSprite_ = std::make_unique<Sprite>();
    idouSprite_->Initialize("idou.png");
    idouSprite_->SetPos({ 450.0f, 892.0f });
    idouSprite_->SetSize({ 150.0f, 50.0f });

    // ポーズ操作ヒント初期化（HPバーの下、画面左上付近）
    pauseHintIconSprite_ = std::make_unique<Sprite>();
    pauseHintIconSprite_->Initialize("button/Menu_Button_Up.png");
    pauseHintIconSprite_->SetPos({ 30.0f, 157.0f });
    pauseHintIconSprite_->SetSize({ 80.0f, 80.0f });

    pauseHintTextSprite_ = std::make_unique<Sprite>();
    pauseHintTextSprite_->Initialize("PauseMenu_Text.png");
    pauseHintTextSprite_->SetPos({ 124.0f, 120.0f });
    pauseHintTextSprite_->SetSize({ 240.0f, 120.0f });

    // リサイズコールバック登録
    winApp_ = WinApp::GetInstance();
    if (winApp_) {
        onResizeId_ = winApp_->RegisterOnResizeFunc(
            std::bind(&ControllerUI::OnResize, this, std::placeholders::_1)
        );

        // 現在のウィンドウサイズに合わせて位置調整（シーン再入場対応）
        Vector2 currentSize = { static_cast<float>(WinApp::clientWidth), static_cast<float>(WinApp::clientHeight) };
        if (currentSize.x != kBaseWidth || currentSize.y != kBaseHeight) {
            OnResize(currentSize);
        }
    }
}

void ControllerUI::Update()
{
    Input* input = Input::GetInstance();

    // ボタン状態更新
    isAPressed_ = input->PushButton(XButtons.A);
    isBPressed_ = input->PushButton(XButtons.B);
    isXPressed_ = input->PushButton(XButtons.X);
    isYPressed_ = input->PushButton(XButtons.Y);

    // スティック方向更新
    Vector2 leftStick = input->GetLeftStick();
    Vector2 rightStick = input->GetRightStick();
    leftStickDir_ = GetStickDirectionIndex(leftStick);
    rightStickDir_ = GetStickDirectionIndex(rightStick);

    // ボタンスプライトの更新
    aButtonUpSprite_->Update();
    aButtonDownSprite_->Update();
    bButtonUpSprite_->Update();
    bButtonDownSprite_->Update();
    xButtonUpSprite_->Update();
    xButtonDownSprite_->Update();
    yButtonUpSprite_->Update();
    yButtonDownSprite_->Update();

    // ジョイスティックスプライトの更新
    for (int i = 0; i < 8; ++i) {
        lJoystickSprites_[i]->Update();
        rJoystickSprites_[i]->Update();
    }

    // アクションアイコンの更新
    kougekiSprite_->Update();
    dashSprite_->Update();
    parrySprite_->Update();
    shagekiSprite_->Update();
    idouSprite_->Update();

    // ポーズ操作ヒントの更新
    pauseHintIconSprite_->Update();
    pauseHintTextSprite_->Update();
}

int ControllerUI::GetStickDirectionIndex(const Vector2& stick) const
{
    // スティック入力の大きさを計算
    float magnitude = std::sqrt(stick.x * stick.x + stick.y * stick.y);

    // デッドゾーン判定：入力が閾値未満ならデフォルト方向（上）
    if (magnitude < stickDeadzone_) {
        return 0;
    }

    // 角度計算
    // atan2(x, y)で「上」を0度基準に変換
    // 上→左上→左→左下→下→右下→右→右上
    float angle = std::atan2(stick.x, stick.y) * 180.0f / 3.14159265f;

    // 22.5度オフセットを加えて、境界を方向の「間」に配置
    angle += 22.5f;
    if (angle < 0.0f) {
        angle += 360.0f;
    }

    // 8方向に分類（各方向45度ずつ）
    int index = static_cast<int>(angle / 45.0f) % 8;
    return index;
}

void ControllerUI::Draw()
{
    // ポーズ中はポーズ操作ヒントのみ描画（操作説明UIはPauseMenuが描画）
    if (isPaused_) {
        pauseHintIconSprite_->Draw();
        pauseHintTextSprite_->Draw();
        return;
    }

    // === 以下、通常時の描画 ===

    // フェーズ2判定（ボス参照が有効かつフェーズ2の場合）
    bool isPhase2 = boss_ && boss_->GetPhase() == 2;

    // ボタン描画（押されているかどうかで切り替え）
    if (isAPressed_) {
        aButtonDownSprite_->Draw();
    } else {
        aButtonUpSprite_->Draw();
    }

    if (isBPressed_) {
        bButtonDownSprite_->Draw();
    } else {
        bButtonUpSprite_->Draw();
    }

    if (isXPressed_) {
        xButtonDownSprite_->Draw();
    } else {
        xButtonUpSprite_->Draw();
    }

    if (isYPressed_) {
        yButtonDownSprite_->Draw();
    } else {
        yButtonUpSprite_->Draw();
    }

    // 左ジョイスティック描画（常に表示）
    lJoystickSprites_[leftStickDir_]->Draw();

    // 右ジョイスティック描画（フェーズ2以外で表示）
    if (!isPhase2) {
        rJoystickSprites_[rightStickDir_]->Draw();
    }

    // アクションアイコン描画
    kougekiSprite_->Draw();
    dashSprite_->Draw();
    parrySprite_->Draw();

    // 射撃アイコン描画（フェーズ2以外で表示）
    if (!isPhase2) {
        shagekiSprite_->Draw();
    }

    idouSprite_->Draw();

    // ポーズ操作ヒント描画（通常時も表示）
    pauseHintIconSprite_->Draw();
    pauseHintTextSprite_->Draw();
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
        const char* dirNames[] = { "Up", "UpLeft", "Left", "DownLeft", "Down", "DownRight", "Right", "UpRight" };
        ImGui::Text("  Left Stick: %s (index: %d)", dirNames[leftStickDir_], leftStickDir_);
        ImGui::Text("  Right Stick: %s (index: %d)", dirNames[rightStickDir_], rightStickDir_);

        ImGui::Separator();
        ImGui::SliderFloat("Deadzone", &stickDeadzone_, 0.0f, 1.0f);

        if (ImGui::TreeNode("Button Sprites")) {
            if (ImGui::TreeNode("A Button Up")) {
                aButtonUpSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("A Button Down")) {
                aButtonDownSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("B Button Up")) {
                bButtonUpSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("B Button Down")) {
                bButtonDownSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("X Button Up")) {
                xButtonUpSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("X Button Down")) {
                xButtonDownSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Y Button Up")) {
                yButtonUpSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Y Button Down")) {
                yButtonDownSprite_->DrawImGui();
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Joystick Sprites")) {
            for (int i = 0; i < 8; ++i) {
                std::string lName = "L Joystick " + std::to_string(i + 1);
                if (ImGui::TreeNode(lName.c_str())) {
                    lJoystickSprites_[i]->DrawImGui();
                    ImGui::TreePop();
                }
            }
            for (int i = 0; i < 8; ++i) {
                std::string rName = "R Joystick " + std::to_string(i + 1);
                if (ImGui::TreeNode(rName.c_str())) {
                    rJoystickSprites_[i]->DrawImGui();
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Action Icons")) {
            if (ImGui::TreeNode("Kougeki")) {
                kougekiSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Dash")) {
                dashSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Parry")) {
                parrySprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Shageki")) {
                shagekiSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Idou")) {
                idouSprite_->DrawImGui();
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Pause Hint")) {
            if (ImGui::TreeNode("Icon")) {
                pauseHintIconSprite_->DrawImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Text")) {
                pauseHintTextSprite_->DrawImGui();
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
#endif
}

void ControllerUI::OnResize(const Vector2& newSize)
{
    // スケールファクターを計算
    float scaleX = newSize.x / kBaseWidth;
    float scaleY = newSize.y / kBaseHeight;

    // Aボタン
    aButtonUpSprite_->SetPos({ 1565.0f * scaleX, 948.0f * scaleY });
    aButtonDownSprite_->SetPos({ 1565.0f * scaleX, 948.0f * scaleY });

    // Bボタン
    bButtonUpSprite_->SetPos({ 1640.0f * scaleX, 869.0f * scaleY });
    bButtonDownSprite_->SetPos({ 1640.0f * scaleX, 869.0f * scaleY });

    // Xボタン
    xButtonUpSprite_->SetPos({ 1488.0f * scaleX, 869.0f * scaleY });
    xButtonDownSprite_->SetPos({ 1488.0f * scaleX, 869.0f * scaleY });

    // Yボタン
    yButtonUpSprite_->SetPos({ 1565.0f * scaleX, 803.0f * scaleY });
    yButtonDownSprite_->SetPos({ 1565.0f * scaleX, 803.0f * scaleY });

    // ジョイスティック（8方向）
    for (int i = 0; i < 8; ++i) {
        lJoystickSprites_[i]->SetPos({ 323.0f * scaleX, 869.0f * scaleY });

        rJoystickSprites_[i]->SetPos({ 791.0f * scaleX, 869.0f * scaleY });
    }

    // アクションアイコン
    kougekiSprite_->SetPos({ 1349.0f * scaleX, 871.0f * scaleY });

    dashSprite_->SetPos({ 1518.0f * scaleX, 997.0f * scaleY });

    parrySprite_->SetPos({ 1701.0f * scaleX, 871.0f * scaleY });

    shagekiSprite_->SetPos({ 913.0f * scaleX, 892.0f * scaleY });

    idouSprite_->SetPos({ 450.0f * scaleX, 892.0f * scaleY });

    // ポーズ操作ヒント
    pauseHintIconSprite_->SetPos({ 30.0f * scaleX, 157.0f * scaleY });

    pauseHintTextSprite_->SetPos({ 124.0f * scaleX, 120.0f * scaleY });
}
