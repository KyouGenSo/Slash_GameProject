#pragma once
#include "GameSettings.h"
#include "SettingsTypes.h"
#include "Vector3.h"

#include <numbers>
#include <vector>

/// <summary>
/// プレイヤーと、その状態・弾の調整値
/// </summary>
TCLASS()
class PlayerSettings : public Tako::GameSettings {
    GENERATED_BODY()

public: //構造体
    /// <summary>
    /// 攻撃ブロックの回転軸
    /// </summary>
    TENUM()
    enum class SwingAxis {
        Horizontal, ///< 水平回転（XZ 平面）
        Vertical    ///< 垂直回転（プレイヤー向き基準の YZ 平面）
    };

    /// <summary>
    /// コンボ 1 段分の攻撃
    /// </summary>
    TSTRUCT()
    struct ComboStep {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, Angle)
        float     startAngle     = 0.0f;
        TPROPERTY(Edit, Angle, Tooltip = "振り幅")
        float     swingAngle     = std::numbers::pi_v<float>;
        TPROPERTY(Edit, Tooltip = "+1 / -1 で回す向き", ClampMin = -1, ClampMax = 1, Speed = 2)
        float     swingDirection = 1.0f;
        TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.01)
        float     attackDuration = 0.15f;
        TPROPERTY(Edit, ClampMin = 0, Speed = 0.5)
        float     damage         = 5.0f;
        TPROPERTY(Edit)
        SwingAxis axis           = SwingAxis::Horizontal;
    };

    /// <summary>
    /// 攻撃状態（ボスへの吸着と前方ステップ）
    /// </summary>
    TSTRUCT()
    struct Attack {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, Tooltip = "吸着移動の最長時間（秒）", ClampMin = 0, Speed = 0.01)
        float moveTime         = 0.8f;
        TPROPERTY(Edit, Tooltip = "前方範囲の片側角（度）", ClampMin = 0, ClampMax = 180, Speed = 1)
        float lungeAngle       = 60.0f;
        TPROPERTY(Edit, Tooltip = "前方範囲の最大距離", ClampMin = 0)
        float lungeMaxDistance = 20.0f;
        TPROPERTY(Edit, Tooltip = "範囲内にボスがいないときの前方ステップ距離", ClampMin = 0)
        float stepDistance     = 5.0f;
        TPROPERTY(Edit, Tooltip = "プレイヤーから攻撃ブロックまでの距離", ClampMin = 0)
        float blockRadius      = 4.0f;
        TPROPERTY(Edit, ClampMin = 0, Speed = 0.01)
        float blockScale       = 0.5f;
        TPROPERTY(Edit, Tooltip = "コンボ完走時の硬直（秒）", ClampMin = 0, Speed = 0.01)
        float recoveryTime     = 0.5f;
    };

    TSTRUCT()
    struct Dash {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.01)
        float duration = 0.05f;
        TPROPERTY(Edit, ClampMin = 0)
        float speed    = 10.0f;
        TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.01)
        float cooldown = 0.5f;
    };

    TSTRUCT()
    struct Parry {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, Tooltip = "受付時間（秒）", ClampMin = 0, Speed = 0.01)
        float duration          = 0.5f;
        TPROPERTY(Edit, Tooltip = "成功したときの回復量", ClampMin = 0)
        float successHealAmount = 5.0f;
        TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.01)
        float cooldown          = 1.0f;
    };

    TSTRUCT()
    struct Shoot {
        GENERATED_STRUCT_BODY()

        TPROPERTY(Edit, Tooltip = "発射の間隔（秒）", ClampMin = 0, Speed = 0.01)
        float fireRate            = 0.2f;
        TPROPERTY(Edit, Tooltip = "射撃中の移動速度の倍率", ClampMin = 0, Speed = 0.01)
        float moveSpeedMultiplier = 0.5f;
        TPROPERTY(Edit, ClampMin = 0, Speed = 0.01)
        float aimRotationLerp     = 0.3f;
        TPROPERTY(Edit)
        bool  aimAssistEnabled    = true;
        TPROPERTY(Edit, Tooltip = "度", ClampMin = 0, ClampMax = 90, Speed = 0.5)
        float aimAssistAngle      = 30.0f;
        TPROPERTY(Edit, ClampMin = 0, ClampMax = 1, Speed = 0.01)
        float aimAssistStrength   = 1.0f;
        TPROPERTY(Edit, ClampMin = 0)
        float bulletSpeed         = 30.0f;
    };

public: //メンバー関数
    void PostEditChange() override { KeepOneCombo(); }

    void PostLoad([[maybe_unused]] const nlohmann::json& raw) override { KeepOneCombo(); }

private: //非公開関数
    /// <summary>
    /// コンボが空だと攻撃状態が 1 段目を読めないので、1 段は残す
    /// </summary>
    void KeepOneCombo()
    {
        if (combos.empty()) { combos.resize(1); }
    }

public: //メンバー変数
    TPROPERTY(Edit, Category = "Health", ClampMin = 1, Speed = 1)
    float maxHp = 185.0f;

    TPROPERTY(Edit, Category = "Move", Speed = 0.01)
    float speed             = 0.5f;
    TPROPERTY(Edit, Category = "Move", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float moveInputDeadzone = 0.1f;
    TPROPERTY(Edit, Category = "Move", ClampMin = 0, Speed = 0.01)
    float rotationLerpSpeed = 0.2f;

    TPROPERTY(Edit, Category = "Attack Move", Tooltip = "攻撃を始めるボスとの距離", ClampMin = 0)
    float attackStartDistance    = 5.0f;
    TPROPERTY(Edit, Category = "Attack Move", ClampMin = 0, Speed = 0.01)
    float attackMoveRotationLerp = 0.3f;
    TPROPERTY(Edit, Category = "Attack Move", ClampMin = 0, Speed = 0.01)
    float bossLookatLerp         = 1.15f;
    TPROPERTY(Edit, Category = "Attack Move", ClampMin = 0)
    float attackMoveSpeed        = 30.0f;

    TPROPERTY(Edit, Category = "Collider", Tooltip = "近接攻撃の箱の全長", ClampMin = 0, Speed = 0.05)
    Tako::Vector3 meleeColliderSize    = { 5.0f, 2.0f, 17.0f };
    TPROPERTY(Edit, Category = "Collider", Tooltip = "近接攻撃の箱の前方へのずれ", Speed = 0.05)
    float         meleeColliderOffsetZ = 10.0f;

    TPROPERTY(Edit, Category = "States")
    Attack attack;
    TPROPERTY(Edit, Category = "States", Tooltip = "段数がそのまま最大コンボ数になる")
    std::vector<ComboStep> combos = {
        { -std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float>, 1.0f, 0.15f, 5.0f, SwingAxis::Horizontal },
        { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float>, -1.0f, 0.15f, 5.0f, SwingAxis::Horizontal },
        { -std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float>, 1.0f, 0.2f, 5.0f, SwingAxis::Vertical },
        { 0.0f, std::numbers::pi_v<float> * 2.0f, -1.0f, 0.3f, 10.0f, SwingAxis::Horizontal },
    };
    TPROPERTY(Edit, Category = "States")
    Dash dash;
    TPROPERTY(Edit, Category = "States")
    Parry parry;
    TPROPERTY(Edit, Category = "States")
    Shoot shoot;

};
