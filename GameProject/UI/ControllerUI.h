#pragma once
#include <memory>
#include <array>
#include "Sprite.h"
#include "Vector2.h"

// 前方宣言
class Boss;

/// <summary>
/// コントローラーUI表示クラス
/// ゲームパッドの入力状態を視覚的に表示
/// ボタン押下でUp/Down切り替え、スティック方向に応じたスプライト切り替えを行う
/// </summary>
class ControllerUI
{
public:
    ControllerUI() = default;
    ~ControllerUI() = default;

    /// <summary>
    /// 初期化
    /// 全スプライトのロードと初期位置設定
    /// </summary>
    void Initialize();

    /// <summary>
    /// 更新
    /// 入力状態に応じてスプライト切り替えフラグを更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// 現在の入力状態に応じたスプライトを描画
    /// </summary>
    void Draw();

    /// <summary>
    /// ImGuiでパラメータ調整
    /// 各スプライトの位置・サイズ等を調整可能
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// ボスへの参照を設定（フェーズ判定用）
    /// </summary>
    /// <param name="boss">ボスへのポインタ</param>
    void SetBoss(Boss* boss) { boss_ = boss; }

    /// <summary>
    /// ポーズ状態を設定
    /// </summary>
    /// <param name="isPaused">ポーズ中かどうか</param>
    void SetIsPaused(bool isPaused) { isPaused_ = isPaused; }

private:
    /// <summary>
    /// スティック入力から方向インデックス(0-7)を取得
    /// </summary>
    /// <param name="stick">スティック入力ベクトル</param>
    /// <returns>方向インデックス (0:上, 1:左上, 2:左, 3:左下, 4:下, 5:右下, 6:右, 7:右上)</returns>
    int GetStickDirectionIndex(const Tako::Vector2& stick) const;

    // ボタンスプライト（Up/Down各4ボタン）
    std::unique_ptr<Tako::Sprite> aButtonUpSprite_;
    std::unique_ptr<Tako::Sprite> aButtonDownSprite_;
    std::unique_ptr<Tako::Sprite> bButtonUpSprite_;
    std::unique_ptr<Tako::Sprite> bButtonDownSprite_;
    std::unique_ptr<Tako::Sprite> xButtonUpSprite_;
    std::unique_ptr<Tako::Sprite> xButtonDownSprite_;
    std::unique_ptr<Tako::Sprite> yButtonUpSprite_;
    std::unique_ptr<Tako::Sprite> yButtonDownSprite_;

    // ジョイスティックスプライト（左右各8方向）
    std::array<std::unique_ptr<Tako::Sprite>, 8> lJoystickSprites_;
    std::array<std::unique_ptr<Tako::Sprite>, 8> rJoystickSprites_;

    // アクションアイコンスプライト
    std::unique_ptr<Tako::Sprite> kougekiSprite_;
    std::unique_ptr<Tako::Sprite> dashSprite_;
    std::unique_ptr<Tako::Sprite> parrySprite_;
    std::unique_ptr<Tako::Sprite> shagekiSprite_;
    std::unique_ptr<Tako::Sprite> idouSprite_;

    // ポーズ操作ヒントスプライト
    std::unique_ptr<Tako::Sprite> pauseHintIconSprite_;   ///< Menuボタンアイコン
    std::unique_ptr<Tako::Sprite> pauseHintTextSprite_;   ///< PAUSEテキスト

    // 現在の表示状態
    bool isAPressed_ = false;
    bool isBPressed_ = false;
    bool isXPressed_ = false;
    bool isYPressed_ = false;
    int leftStickDir_ = 0;   ///< 左スティック方向インデックス (0-7)
    int rightStickDir_ = 0;  ///< 右スティック方向インデックス (0-7)

    // デッドゾーン閾値（スティック入力がこの値未満なら無視）
    float stickDeadzone_ = 0.3f;

    // 状態参照用
    Boss* boss_ = nullptr;  ///< ボスへの参照（フェーズ判定用）
    bool isPaused_ = false; ///< ポーズ状態
};
