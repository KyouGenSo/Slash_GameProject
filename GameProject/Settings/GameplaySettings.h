#pragma once
#include "GameSettings.h"
#include "SettingsTypes.h"

/// <summary>
/// 入力・演出など、持ち主が 1 つに決まらない調整値
/// </summary>
TCLASS()
class GameplaySettings : public Tako::GameSettings {
    GENERATED_BODY()

public: //メンバー変数
    TPROPERTY(Edit, Category = "Input", Tooltip = "トリガーを押したとみなす深さ", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float triggerThreshold = 0.5f;

    TPROPERTY(Edit, Category = "Effect", Tooltip = "ダッシュの粒子がプレイヤーに追いつく速さ。0 以下なら DashEffectManager の値", Speed = 0.5)
    float         dashEffectLerpSpeed = 35.0f;
    TPROPERTY(Edit, Category = "Effect", Tooltip = "カメラの揺れ")
    ShakeSettings cameraShake         = { 0.3f, 0.5f };
    TPROPERTY(Edit, Category = "Effect", Tooltip = "ボスに攻撃が当たったときに止める時間（秒）", ClampMin = 0, Speed = 0.01)
    float         hitStopDuration     = 0.08f;
};
