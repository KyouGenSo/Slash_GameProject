#pragma once
#include <array>
#include <memory>
#include <cstdint>

namespace Tako {
class Sprite;
class WinApp;
struct Vector2;
}

/// <summary>
/// ポーズメニュークラス
/// ゲーム中断時のメニュー表示・操作を管理
/// Resume/Title/Exit の3ボタンを DPAD で選択、A ボタンで決定
/// </summary>
class PauseMenu
{
public:
    /// <summary>
    /// メニュー選択結果
    /// </summary>
    enum class Action {
        None,      ///< 何も選択されていない
        Resume,    ///< ゲーム再開
        ToTitle,   ///< タイトル画面へ
        ExitGame   ///< ゲーム終了
    };

    PauseMenu() = default;
    ~PauseMenu();

    /// <summary>
    /// 初期化
    /// 全スプライトのロードと初期位置設定
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// 入力を検出し、選択されたアクションを返す
    /// </summary>
    /// <returns>選択されたアクション（決定されていなければ None）</returns>
    Action Update();

    /// <summary>
    /// 描画
    /// 半透明オーバーレイ、タイトル、ボタンを描画
    /// </summary>
    void Draw();

    /// <summary>
    /// 選択インデックスをリセット
    /// メニュー再表示時に Resume が選択された状態に戻す
    /// </summary>
    void Reset();

    /// <summary>
    /// ImGui でパラメータ調整
    /// 各スプライトの位置・サイズ等を調整可能
    /// </summary>
    void DrawImGui();

private:
    /// <summary>
    /// ボタンの色を更新
    /// 選択中のボタンは白、それ以外はグレー
    /// </summary>
    void UpdateButtonColors();

    /// <summary>
    /// ウィンドウリサイズ時のコールバック
    /// 全スプライトの位置・サイズを新しいウィンドウサイズに合わせて再計算
    /// </summary>
    /// <param name="newSize">新しいウィンドウサイズ</param>
    void OnResize(const Tako::Vector2& newSize);

private:

    // ボタンスプライト（Resume, Title, Exit）
    static constexpr int kButtonCount = 3;
    std::array<std::unique_ptr<Tako::Sprite>, kButtonCount> buttonSprites_;

    // タイトルスプライト（PAUSE 表示）
    std::unique_ptr<Tako::Sprite> titleSprite_;

    // 半透明黒背景オーバーレイ
    std::unique_ptr<Tako::Sprite> overlaySprite_;

    // DPAD 操作ガイドスプライト
    std::unique_ptr<Tako::Sprite> dpadGuideSprite_;   ///< DPAD 中立状態
    std::unique_ptr<Tako::Sprite> dpadUpSprite_;      ///< DPAD 上押下
    std::unique_ptr<Tako::Sprite> dpadDownSprite_;    ///< DPAD 下押下

    // A ボタンスプライト
    std::unique_ptr<Tako::Sprite> aButtonUpSprite_;   ///< A ボタン通常
    std::unique_ptr<Tako::Sprite> aButtonDownSprite_; ///< A ボタン押下

    // テキストスプライト
    std::unique_ptr<Tako::Sprite> ketteiSprite_;      ///< 決定テキスト
    std::unique_ptr<Tako::Sprite> sentakuSprite_;     ///< 選択テキスト

    // 入力状態（描画用）
    bool isDPadUpPressed_ = false;   ///< DPAD 上ボタン押下状態
    bool isDPadDownPressed_ = false; ///< DPAD 下ボタン押下状態
    bool isAPressed_ = false;        ///< A ボタン押下状態

    // 現在選択中のボタンインデックス（0:Resume, 1:Title, 2:Exit）
    int selectedIndex_ = 0;

    // 選択時の色（白）
    static constexpr float kSelectedColorR = 1.0f;
    static constexpr float kSelectedColorG = 1.0f;
    static constexpr float kSelectedColorB = 1.0f;

    // 非選択時の色（グレー）
    static constexpr float kUnselectedColorR = 0.5f;
    static constexpr float kUnselectedColorG = 0.5f;
    static constexpr float kUnselectedColorB = 0.5f;

    // 基準解像度定数（UI 設計時の想定解像度）
    static constexpr float kBaseWidth = 1920.0f;
    static constexpr float kBaseHeight = 1080.0f;

    // リサイズコールバック管理
    Tako::WinApp* winApp_ = nullptr;   ///< WinApp への参照
    uint32_t onResizeId_ = 0;          ///< 登録されたコールバックの ID
};
