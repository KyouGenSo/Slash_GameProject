#pragma once
#include <array>
#include <memory>

namespace Tako {
class Sprite;
}

/// <summary>
/// ポーズメニュークラス
/// ゲーム中断時のメニュー表示・操作を管理
/// Resume/Title/Exitの3ボタンをDPADで選択、Aボタンで決定
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
    ~PauseMenu() = default;

    /// <summary>
    /// 初期化
    /// 全スプライトのロードと初期位置設定
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新処理
    /// 入力を検出し、選択されたアクションを返す
    /// </summary>
    /// <returns>選択されたアクション（決定されていなければNone）</returns>
    Action Update();

    /// <summary>
    /// 描画
    /// 半透明オーバーレイ、タイトル、ボタンを描画
    /// </summary>
    void Draw();

    /// <summary>
    /// 選択インデックスをリセット
    /// メニュー再表示時にResumeが選択された状態に戻す
    /// </summary>
    void Reset();

private:
    /// <summary>
    /// ボタンの色を更新
    /// 選択中のボタンは白、それ以外はグレー
    /// </summary>
    void UpdateButtonColors();

    // ボタンスプライト（Resume, Title, Exit）
    static constexpr int kButtonCount = 3;
    std::array<std::unique_ptr<Tako::Sprite>, kButtonCount> buttonSprites_;

    // タイトルスプライト（PAUSE表示）
    std::unique_ptr<Tako::Sprite> titleSprite_;

    // 半透明黒背景オーバーレイ
    std::unique_ptr<Tako::Sprite> overlaySprite_;

    // DPAD操作ガイドスプライト
    std::unique_ptr<Tako::Sprite> dpadGuideSprite_;

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
};
