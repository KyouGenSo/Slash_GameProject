#pragma once
#include "Script.h"
#include "UILayout.h"
#include "Vector2.h"

// 前方宣言
class Boss;

/// <summary>
/// ゲームパッドの入力状態を表示する UI。同じ Actor の UILayoutComponent の画像を、
/// ボタン押下で Up/Down、スティック方向で切り替える（配置は resources/Json/UI/ControllerUI.json）
/// </summary>
TCLASS(Requires = "UILayoutComponent")
class ControllerUI : public Tako::Script {
    GENERATED_BODY()

private: //定数
    static constexpr int kStickDirectionCount = 8;

public: //メンバー関数
    void BeginPlay() override;

    void Tick(float deltaTime) override;

    /// <summary>
    /// ポーズ中も、押したボタンの画像を切り替える
    /// </summary>
    bool TickWhilePaused() const override { return true; }

#ifdef _DEBUG
    /// <summary>
    /// 今のボタンとスティックの状態
    /// </summary>
    bool DrawCustomImGui() override;
#endif

    //=============================
    //Setter
    //=============================
    /// <summary>
    /// フェーズ判定用にボス参照を設定
    /// </summary>
    void SetBoss(Boss* boss) { boss_ = boss; }

    /// <summary>
    /// ポーズ中は操作ヒント以外を隠す
    /// </summary>
    void SetIsPaused(bool isPaused);

private: //非公開関数
    /// <summary>
    /// スティック入力を8方向のインデックスへ量子化する
    /// </summary>
    /// <param name="stick">スティック入力。x=左右(右が正)、y=上下(上が正)</param>
    /// <returns>上を0とし時計回りに増える方向インデックス (0:上, 1:右上, 2:右, 3:右下, 4:下, 5:左下, 6:左, 7:左上)。デッドゾーン未満は0</returns>
    int GetStickDirectionIndex(const Tako::Vector2& stick) const;

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "スティック入力がこの値未満なら無視する", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float stickDeadzone_ = 0.3f;

    Tako::UILayout* layout_ = nullptr;  ///< 非所有（同じ Actor の UILayoutComponent の物）。無ければ何もしない

    // 現在の表示状態
    bool isAPressed_    = false;
    bool isBPressed_    = false;
    bool isXPressed_    = false;
    bool isYPressed_    = false;
    int  leftStickDir_  = 0;      ///< 0-7
    int  rightStickDir_ = 0;      ///< 0-7

    Boss* boss_     = nullptr;  ///< フェーズ判定用
    bool  isPaused_ = false;
};
