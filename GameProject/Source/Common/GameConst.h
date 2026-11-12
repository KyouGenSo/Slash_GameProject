#pragma once
#include "Source/Settings/GameplaySettings.h"

/// <summary>
/// 固定値の定数と、GameplaySettings のステージの値を読む関数。調整が必要なパラメータは GameSettings（Settings/ の各クラス）を使う
/// </summary>
namespace GameConst {

    // ステージ範囲（GameplaySettings の Stage）
    inline float StageXMin() { return Tako::GameSettings::Get<GameplaySettings>().stageRangeX.x; }
    inline float StageXMax() { return Tako::GameSettings::Get<GameplaySettings>().stageRangeX.y; }
    inline float StageZMin() { return Tako::GameSettings::Get<GameplaySettings>().stageRangeZ.x; }
    inline float StageZMax() { return Tako::GameSettings::Get<GameplaySettings>().stageRangeZ.y; }

    /// <summary>
    /// これより短いベクトルは方向なしとみなす閾値
    /// </summary>
    inline constexpr float kDirectionEpsilon = 0.01f;

    /// <summary>
    /// ボスがステージ端に寄りすぎないためのエリア境界マージン
    /// </summary>
    inline constexpr float kAreaMargin = 10.0f;

    /// <summary>
    /// ボスフェーズ2の戦闘エリアの範囲（GameplaySettings の Stage）
    /// </summary>
    inline float BossPhase2AreaSize() { return Tako::GameSettings::Get<GameplaySettings>().bossPhase2AreaSize; }
}
