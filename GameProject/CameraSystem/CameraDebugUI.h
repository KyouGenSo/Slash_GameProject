#pragma once
#include "CameraManager.h"
#include "Controller/ThirdPersonController.h"
#include "Controller/TopDownController.h"

#ifdef _DEBUG

/// <summary>
/// カメラシステムの ImGui デバッグ UI。調整値は GameCamera のプロパティで編集するので、ここは状態の表示と切り替えだけ
/// </summary>
class CameraDebugUI {
public: //メンバー関数
    /// <summary>
    /// 今のウィンドウの中に描く（GameCamera の Inspector から呼ぶ。自分ではウィンドウを開かない）
    /// </summary>
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
