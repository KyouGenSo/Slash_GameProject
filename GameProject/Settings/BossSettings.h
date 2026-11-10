#pragma once
#include "GameSettings.h"
#include "SettingsTypes.h"
#include "Vector3.h"

/// <summary>
/// ボスと、その近接攻撃・弾の調整値（行動ごとの値は BossTree.json の BT ノードが持つ）
/// </summary>
TCLASS()
class BossSettings : public Tako::GameSettings {
    GENERATED_BODY()

public: //構造体
    /// <summary>
    /// 近接攻撃の当たり判定
    /// </summary>
    TSTRUCT()
    struct MeleeCollider {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, ClampMin = 0, Speed = 0.5)
        float         damage  = 10.0f;
        TPROPERTY(Edit, Tooltip = "箱の全長", ClampMin = 0, Speed = 0.05)
        Tako::Vector3 size    = { 2.0f, 2.0f, 2.0f };
        TPROPERTY(Edit, Tooltip = "前方へのずれ", Speed = 0.05)
        float         offsetZ = 3.0f;
    };

public: //メンバー変数
    TPROPERTY(Edit, Category = "Health", ClampMin = 1, Speed = 1)
    float maxHp            = 200.0f;
    TPROPERTY(Edit, Category = "Health", Tooltip = "HP がこの値以下になるとフェーズ 2 に入る", ClampMin = 0, Speed = 1)
    float phase2Threshold  = 105.0f;
    TPROPERTY(Edit, Category = "Health", Tooltip = "フェーズ 2 に入ったときの HP", ClampMin = 0, Speed = 1)
    float phase2InitialHp  = 100.0f;

    TPROPERTY(Edit, Category = "Body", Tooltip = "被弾で白く光る時間（秒）", ClampMin = 0, Speed = 0.01)
    float         hitEffectDuration = 0.1f;
    TPROPERTY(Edit, Category = "Body", Tooltip = "被弾時の揺れ")
    ShakeSettings shake             = { 0.3f, 0.2f };

    TPROPERTY(Edit, Category = "Attack")
    MeleeCollider melee;

};
