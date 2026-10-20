#pragma once
#include "CameraManager.h"
#include "Controller/ThirdPersonController.h"
#include "Controller/TopDownController.h"

#ifdef _DEBUG

/// <summary>
/// カメラシステムの ImGui デバッグ UI
/// </summary>
class CameraDebugUI {
public: //メンバー関数
    static void Draw();
    static void DrawManagerInfo();
    static void DrawFirstPersonControllerInfo(ThirdPersonController* controller);
    static void DrawTopDownControllerInfo(TopDownController* controller);

private: //非公開関数
    static void DrawControllerSwitcher();

private: //メンバー変数
    static bool showManagerInfo_;
    static bool showControllerInfo_;
    static bool showAnimationInfo_;
};

#endif // _DEBUG
