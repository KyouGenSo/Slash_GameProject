#pragma once
#include "Script.h"
#include "UILayout.h"
#include <array>

/// <summary>
/// ポーズメニュー。Resume/Title/Exit を DPAD で選択、A ボタンで決定。画像は同じ Actor の UILayoutComponent の物を使う。
/// 自分では Tick せず、ポーズの間だけ進行役（GameDirector）が Update を呼んで結果を受け取る
/// </summary>
TCLASS()
class PauseMenu : public Tako::Script {
    GENERATED_BODY()

private: //定数
    static constexpr int kButtonCount = 3;

    // 選択時の色（白）
    static constexpr float kSelectedColorR = 1.0f;
    static constexpr float kSelectedColorG = 1.0f;
    static constexpr float kSelectedColorB = 1.0f;

    // 非選択時の色（グレー）
    static constexpr float kUnselectedColorR = 0.5f;
    static constexpr float kUnselectedColorG = 0.5f;
    static constexpr float kUnselectedColorB = 0.5f;

    // selectedIndex_ の順。resources/Json/UI/PauseMenu.json の要素名と一致させる
    static constexpr std::array<const char*, kButtonCount> kButtonNames = { "Resume", "ToTitle", "Exit" };

public: //構造体
    enum class Action {
        None,
        Resume,
        ToTitle,
        ExitGame
    };

public: //メンバー関数
    void BeginPlay() override;

    /// <returns>選択されたアクション（未決定なら None）</returns>
    Action Update();

    /// <summary>
    /// 選択を Resume に戻す
    /// </summary>
    void Reset();

#ifdef _DEBUG
    /// <summary>
    /// 今の選択と入力の状態
    /// </summary>
    bool DrawCustomImGui() override;
#endif

private: //非公開関数
    /// <summary>
    /// 選択中は白、それ以外はグレーに設定
    /// </summary>
    void UpdateButtonColors();

private: //メンバー変数
    Tako::UILayout* layout_ = nullptr;  ///< 非所有（同じ Actor の UILayoutComponent の物）。無ければ何もしない

    // 入力状態（描画用）
    bool isDPadUpPressed_   = false;
    bool isDPadDownPressed_ = false;
    bool isAPressed_        = false;

    // 0:Resume, 1:Title, 2:Exit
    int selectedIndex_ = 0;
};
