#pragma once
#include "GameSettings.h"
#include "SettingsTypes.h"
#include "Vector2.h"

/// <summary>
/// ステージ・演出など、持ち主が 1 つに決まらない調整値
/// </summary>
TCLASS()
class GameplaySettings : public Tako::GameSettings {
    GENERATED_BODY()

public: //メンバー変数
    TPROPERTY(Edit, Category = "Stage", Tooltip = "ステージの X の範囲（最小, 最大）。プレイヤー・ボス・弾がこの中に収まる", Speed = 1)
    Tako::Vector2 stageRangeX        = { -100.0f, 100.0f };
    TPROPERTY(Edit, Category = "Stage", Tooltip = "ステージの Z の範囲（最小, 最大）", Speed = 1)
    Tako::Vector2 stageRangeZ        = { -140.0f, 60.0f };
    TPROPERTY(Edit, Category = "Stage", Tooltip = "ボスのフェーズ 2 で戦う範囲の広さ", ClampMin = 0, Speed = 0.5)
    float         bossPhase2AreaSize = 30.0f;

    TPROPERTY(Edit, Category = "Effect", Tooltip = "ダッシュの粒子がプレイヤーに追いつく速さ。0 以下なら DashEffectManager の値", Speed = 0.5)
    float         dashEffectLerpSpeed = 35.0f;
    TPROPERTY(Edit, Category = "Effect", Tooltip = "カメラの揺れ")
    ShakeSettings cameraShake         = { 0.3f, 0.5f };
    TPROPERTY(Edit, Category = "Effect", Tooltip = "ボスに攻撃が当たったときに止める時間（秒）", ClampMin = 0, Speed = 0.01)
    float         hitStopDuration     = 0.08f;
};
