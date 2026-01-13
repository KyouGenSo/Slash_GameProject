#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include "Vector2.h"
#include "Vector4.h"
#include "Sprite.h"

/// <summary>
/// HPバーUIクラス
/// HPバーの初期化・更新・描画を一元管理
/// </summary>
class HPBarUI
{
public:
    HPBarUI() = default;
    ~HPBarUI() = default;

    /// <summary>
    /// 初期化（単一バー）
    /// </summary>
    /// <param name="texture">テクスチャファイル名</param>
    /// <param name="size">バーのサイズ</param>
    /// <param name="screenXRatio">画面X座標（画面幅に対する比率）</param>
    /// <param name="screenYRatio">画面Y座標（画面高さに対する比率）</param>
    /// <param name="barColor">バーの色</param>
    /// <param name="bgColor">背景の色</param>
    void Initialize(
        const std::string& texture,
        const Tako::Vector2& size,
        float screenXRatio,
        float screenYRatio,
        const Tako::Vector4& barColor,
        const Tako::Vector4& bgColor = Tako::Vector4{ 1.f, 1.f, 1.f, 1.0f });

    /// <summary>
    /// 初期化（2段バー、Bossフェーズ用）
    /// </summary>
    /// <param name="texture">テクスチャファイル名</param>
    /// <param name="size">バーのサイズ</param>
    /// <param name="screenXRatio">画面X座標（画面幅に対する比率）</param>
    /// <param name="screenYRatio">画面Y座標（画面高さに対する比率）</param>
    /// <param name="bar1Color">フェーズ1バーの色</param>
    /// <param name="bar2Color">フェーズ2バーの色</param>
    /// <param name="bgColor">背景の色</param>
    void InitializeDual(
        const std::string& texture,
        const Tako::Vector2& size,
        float screenXRatio,
        float screenYRatio,
        const Tako::Vector4& bar1Color,
        const Tako::Vector4& bar2Color,
        const Tako::Vector4& bgColor = Tako::Vector4{ 1.f, 1.f, 1.f, 1.0f });

    /// <summary>
    /// 更新（単一バー用）
    /// </summary>
    /// <param name="currentValue">現在の値</param>
    /// <param name="maxValue">最大値</param>
    void Update(float currentValue, float maxValue);

    /// <summary>
    /// 更新（2段バー用、Bossフェーズ用）
    /// </summary>
    /// <param name="currentHp">現在HP</param>
    /// <param name="maxHp">最大HP</param>
    /// <param name="phase2Threshold">フェーズ2開始閾値</param>
    /// <param name="phase">現在のフェーズ（1 or 2）</param>
    void UpdateDual(float currentHp, float maxHp, float phase2Threshold, uint32_t phase);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// 画面位置を設定
    /// </summary>
    /// <param name="screenXRatio">画面X座標（画面幅に対する比率）</param>
    /// <param name="screenYRatio">画面Y座標（画面高さに対する比率）</param>
    void SetPosition(float screenXRatio, float screenYRatio);

    /// <summary>
    /// バーの色を設定
    /// </summary>
    /// <param name="color">新しい色</param>
    void SetBarColor(const Tako::Vector4& color);

    /// <summary>
    /// アンカーポイントを設定
    /// </summary>
    /// <param name="anchor">アンカーポイント（0,0:左上 1,1:右下）</param>
    void SetAnchorPoint(const Tako::Vector2& anchor);

private:
    /// <summary>
    /// 画面座標を計算
    /// </summary>
    Tako::Vector2 CalculateScreenPosition() const;

    std::unique_ptr<Tako::Sprite> barSprite_;    ///< バースプライト
    std::unique_ptr<Tako::Sprite> bar2Sprite_;   ///< 2段目バースプライト（Dual用）
    std::unique_ptr<Tako::Sprite> bgSprite_;     ///< 背景スプライト

    Tako::Vector2 baseSize_;     ///< 基本サイズ
    float screenXRatio_ = 0.5f;  ///< 画面X比率
    float screenYRatio_ = 0.05f; ///< 画面Y比率

    bool isDualBar_ = false;     ///< 2段バーモードか
};
