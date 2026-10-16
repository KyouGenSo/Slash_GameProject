#pragma once
#include <cstdint>
#include <string>
#include "UILayout.h"
#include "Vector2.h"
#include "Vector4.h"

/// <summary>
/// HP バーの更新・描画を管理（配置・大きさ・色は resources/Json/UI/ のレイアウトで決める）
/// </summary>
class HPBarUI
{
public: //メンバー関数
    /// <summary>
    /// Background と Bar（2段バーなら Bar2 も）を持つレイアウトを読み込む
    /// </summary>
    /// <param name="layoutName">レイアウト名（拡張子なし）</param>
    void Initialize(const std::string& layoutName);

    /// <summary>
    /// 現在値/最大値の比率でバー幅を更新する
    /// </summary>
    /// <param name="currentValue">現在値。maxValue で割った比率は0.0-1.0にクランプ</param>
    /// <param name="maxValue">最大値。0以下なら比率0扱い</param>
    void Update(float currentValue, float maxValue);

    /// <summary>
    /// フェーズに応じて2段バーの幅を更新する
    /// </summary>
    /// <param name="currentHp">現在HP</param>
    /// <param name="maxHp">最大HP</param>
    /// <param name="phase2Threshold">フェーズ2開始HP閾値。これ以上が手前バー、以下が奥バーの担当</param>
    /// <param name="phase">現在のフェーズ（1: 手前バー減少 / 2: 手前バー0幅・奥バー減少）</param>
    void UpdateDual(float currentHp, float maxHp, float phase2Threshold, uint32_t phase);

    void Draw();

    //=======================================================
    //Setter
    //=======================================================
    /// <summary>
    /// 全要素のアンカーを動かす
    /// </summary>
    /// <param name="screenXRatio">画面幅に対する比率</param>
    /// <param name="screenYRatio">画面高さに対する比率</param>
    void SetPosition(float screenXRatio, float screenYRatio);

    void SetBarColor(const Tako::Vector4& color);

    /// <summary>
    /// 全要素の pivot を設定する。バーはこの点を固定して縮む
    /// </summary>
    /// <param name="anchor">0,0:左上 1,1:右下</param>
    void SetAnchorPoint(const Tako::Vector2& anchor);

private: //非公開関数
    /// <summary>
    /// バーの幅を Background の幅 × ratio にする。縮む向きはバーの pivot で決まる
    /// </summary>
    void SetFillRatio(const char* barName, float ratio);

private: //メンバー変数
    Tako::UILayout layout_;
};
