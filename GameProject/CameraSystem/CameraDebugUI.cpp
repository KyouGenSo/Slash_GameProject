#ifdef _DEBUG

#include "CameraDebugUI.h"
#include <imgui.h>
#include <sstream>

// 静的メンバ変数の定義
bool CameraDebugUI::showManagerInfo_ = true;
bool CameraDebugUI::showControllerInfo_ = true;
bool CameraDebugUI::showAnimationInfo_ = true;
bool CameraDebugUI::showPerformanceInfo_ = false;

void CameraDebugUI::Draw() {
    if (!ImGui::Begin("Camera System Debug")) {
        ImGui::End();
        return;
    }

    // メインタブ
    if (ImGui::BeginTabBar("CameraDebugTabs")) {
        // Manager タブ
        if (ImGui::BeginTabItem("Manager")) {
            DrawManagerInfo();
            ImGui::EndTabItem();
        }

        // Controllers タブ
        if (ImGui::BeginTabItem("Controllers")) {
            DrawControllerSwitcher();
            ImGui::EndTabItem();
        }

        // Performance タブ
        if (ImGui::BeginTabItem("Performance")) {
            DrawPerformanceInfo();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void CameraDebugUI::DrawManagerInfo() {
    CameraManager* manager = CameraManager::GetInstance();
    if (!manager) {
        ImGui::Text("CameraManager not initialized");
        return;
    }

    ImGui::Text("=== Camera Manager Status ===");
    ImGui::Text("Controller Count: %zu", manager->GetControllerCount());
    ImGui::Text("Active Controller: %s", manager->GetActiveControllerName().c_str());

    ImGui::Separator();

    // デバッグ情報をテキストで表示
    std::string debugInfo = manager->GetDebugInfo();
    ImGui::TextWrapped("%s", debugInfo.c_str());
}

void CameraDebugUI::DrawFirstPersonControllerInfo(FirstPersonController* controller) {
    if (!controller) {
        return;
    }

    ImGui::Text("=== FirstPerson Controller ===");
    ImGui::Text("Active: %s", controller->IsActive() ? "Yes" : "No");

    if (!controller->IsActive()) {
        if (ImGui::Button("Activate")) {
            controller->Activate();
        }
    } else {
        if (ImGui::Button("Deactivate")) {
            controller->Deactivate();
        }
    }

    ImGui::Separator();

    // オフセット設定
    Vector3 offset = controller->GetOffset();
    float offsetArray[3] = { offset.x, offset.y, offset.z };
    if (ImGui::DragFloat3("Offset", offsetArray, 0.1f)) {
        controller->SetOffset(Vector3(offsetArray[0], offsetArray[1], offsetArray[2]));
    }

    // 回転速度
    static float rotateSpeed = CameraConfig::FirstPerson::DEFAULT_ROTATE_SPEED;
    if (ImGui::SliderFloat("Rotate Speed", &rotateSpeed, 0.01f, 0.2f)) {
        controller->SetRotateSpeed(rotateSpeed);
    }

    // 追従の滑らかさ
    static float smoothness = CameraConfig::FOLLOW_SMOOTHNESS;
    if (ImGui::SliderFloat("Follow Smoothness", &smoothness, 0.01f, 1.0f)) {
        controller->SetSmoothness(smoothness);
    }

    // リセットボタン
    if (ImGui::Button("Reset Camera")) {
        controller->Reset();
    }
}

void CameraDebugUI::DrawTopDownControllerInfo(TopDownController* controller) {
    if (!controller) {
        return;
    }

    ImGui::Text("=== TopDown Controller ===");
    ImGui::Text("Active: %s", controller->IsActive() ? "Yes" : "No");

    if (!controller->IsActive()) {
        if (ImGui::Button("Activate")) {
            controller->Activate();
        }
    } else {
        if (ImGui::Button("Deactivate")) {
            controller->Deactivate();
        }
    }

    ImGui::Separator();

    // カメラ高さ設定
    static float baseHeight = CameraConfig::TopDown::BASE_HEIGHT;
    if (ImGui::DragFloat("Base Height", &baseHeight, 0.5f, 5.0f, 100.0f)) {
        controller->SetBaseHeight(baseHeight);
    }

    // 高さ倍率
    static float heightMultiplier = CameraConfig::TopDown::HEIGHT_MULTIPLIER;
    if (ImGui::SliderFloat("Height Multiplier", &heightMultiplier, 0.0f, 2.0f)) {
        controller->SetHeightMultiplier(heightMultiplier);
    }

    // カメラ角度
    static float angleXDegrees = CameraConfig::TopDown::DEFAULT_ANGLE_X * 57.2958f;
    if (ImGui::SliderFloat("Camera Angle (deg)", &angleXDegrees, 0.0f, 90.0f)) {
        controller->SetCameraAngle(angleXDegrees * 0.0174533f);
    }

    // 追従の滑らかさ
    static float smoothness = CameraConfig::FOLLOW_SMOOTHNESS;
    if (ImGui::SliderFloat("Follow Smoothness", &smoothness, 0.01f, 1.0f)) {
        controller->SetSmoothness(smoothness);
    }

    // 現在の高さ表示
    ImGui::Text("Current Height: %.2f", controller->GetCurrentHeight());

    // リセットボタン
    if (ImGui::Button("Reset Camera")) {
        controller->Reset();
    }
}

void CameraDebugUI::DrawAnimationInfo(CameraAnimation* animation) {
    if (!animation) {
        return;
    }

    ImGui::Text("=== Camera Animation ===");

    // アニメーション情報
    ImGui::Text("Animation: %s", animation->GetAnimationName().c_str());
    ImGui::Text("Duration: %.2f seconds", animation->GetDuration());
    ImGui::Text("Current Time: %.2f", animation->GetCurrentTime());
    ImGui::Text("Keyframes: %zu", animation->GetKeyframeCount());

    // 再生状態
    const char* stateStr = "STOPPED";
    auto playState = animation->GetPlayState();
    if (playState == CameraAnimation::PlayState::PLAYING) stateStr = "PLAYING";
    else if (playState == CameraAnimation::PlayState::PAUSED) stateStr = "PAUSED";
    ImGui::Text("State: %s", stateStr);

    ImGui::Separator();

    // 再生コントロール
    if (ImGui::Button("Play")) {
        animation->Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) {
        animation->Pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        animation->Stop();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        animation->Reset();
    }

    // ループ設定
    bool isLooping = animation->IsLooping();
    if (ImGui::Checkbox("Loop", &isLooping)) {
        animation->SetLooping(isLooping);
    }

    // 再生速度
    static float playSpeed = 1.0f;
    if (ImGui::SliderFloat("Play Speed", &playSpeed, -2.0f, 2.0f, "%.2f")) {
        animation->SetPlaySpeed(playSpeed);
    }

    // タイムラインスライダー
    float currentTime = animation->GetCurrentTime();
    if (ImGui::SliderFloat("Timeline", &currentTime, 0.0f,
                          animation->GetDuration(), "%.2f")) {
        animation->SetCurrentTime(currentTime);
    }
}

void CameraDebugUI::DrawControllerSwitcher() {
    CameraManager* manager = CameraManager::GetInstance();
    if (!manager) {
        return;
    }

    ImGui::Text("=== Controller Switcher ===");

    // アクティブなコントローラーを表示
    std::string activeName = manager->GetActiveControllerName();
    ImGui::Text("Current Active: %s",
               activeName.empty() ? "None" : activeName.c_str());

    ImGui::Separator();

    // FirstPerson/TopDownの簡単切り替え
    if (ImGui::Button("Activate FirstPerson")) {
        manager->DeactivateAllControllers();
        manager->ActivateController("FirstPerson");
    }
    ImGui::SameLine();
    if (ImGui::Button("Activate TopDown")) {
        manager->DeactivateAllControllers();
        manager->ActivateController("TopDown");
    }

    ImGui::Separator();

    // 各コントローラーの詳細情報
    if (ImGui::CollapsingHeader("FirstPerson Controller Details")) {
        auto* fpController = static_cast<FirstPersonController*>(
            manager->GetController("FirstPerson"));
        DrawFirstPersonControllerInfo(fpController);
    }

    if (ImGui::CollapsingHeader("TopDown Controller Details")) {
        auto* tdController = static_cast<TopDownController*>(
            manager->GetController("TopDown"));
        DrawTopDownControllerInfo(tdController);
    }
}

void CameraDebugUI::DrawCameraState() {
    CameraManager* manager = CameraManager::GetInstance();
    if (!manager || !manager->GetCamera()) {
        return;
    }

    Camera* camera = manager->GetCamera();

    ImGui::Text("=== Camera State ===");

    // 位置
    Vector3 pos = camera->GetTranslate();
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);

    // 回転（度単位）
    Vector3 rot = camera->GetRotate();
    ImGui::Text("Rotation: (%.1f°, %.1f°, %.1f°)",
               rot.x * 57.2958f, rot.y * 57.2958f, rot.z * 57.2958f);

    // FOV（度単位）
    float fov = camera->GetFovY() * 57.2958f;
    ImGui::Text("FOV: %.1f°", fov);

    // アスペクト比
    ImGui::Text("Aspect Ratio: %.3f", camera->GetAspect());

    // ニア・ファー
    ImGui::Text("Near/Far: %.2f / %.1f",
               camera->GetNearClip(), camera->GetFarClip());
}

void CameraDebugUI::DrawPerformanceInfo() {
    ImGui::Text("=== Performance Info ===");

    static float updateTimes[60] = {};
    static int updateIndex = 0;
    static float lastUpdateTime = 0.0f;

    // ダミーのパフォーマンスデータ（実装時は実際の計測値を使用）
    updateTimes[updateIndex] = 0.016f; // 仮の値
    updateIndex = (updateIndex + 1) % 60;

    // グラフ表示
    ImGui::PlotLines("Update Time (ms)", updateTimes, 60, updateIndex,
                    "Camera Update", 0.0f, 0.033f, ImVec2(0, 80));

    // 統計情報
    float avgTime = 0.0f;
    for (int i = 0; i < 60; ++i) {
        avgTime += updateTimes[i];
    }
    avgTime /= 60.0f;

    ImGui::Text("Average Update Time: %.3f ms", avgTime * 1000.0f);
    ImGui::Text("FPS Impact: %.1f", avgTime > 0 ? 1.0f / avgTime : 0.0f);
}

#endif // _DEBUG