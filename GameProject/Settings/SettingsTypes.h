#pragma once
#include "Reflection.h"

/// <summary>
/// 弾 1 種類分の調整値
/// </summary>
TSTRUCT()
struct BulletSettings {
    GENERATED_STRUCT_BODY()

    TPROPERTY(Edit, ClampMin = 0, Speed = 0.5)
    float damage         = 10.0f;
    TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.1)
    float lifetime       = 5.0f;
    TPROPERTY(Edit, ClampMin = 0, Speed = 0.05)
    float colliderRadius = 1.0f;
};

/// <summary>
/// 揺れ 1 種類分の調整値
/// </summary>
TSTRUCT()
struct ShakeSettings {
    GENERATED_STRUCT_BODY()

    TPROPERTY(Edit, Tooltip = "秒", ClampMin = 0, Speed = 0.01)
    float duration  = 0.3f;
    TPROPERTY(Edit, ClampMin = 0, Speed = 0.01)
    float intensity = 0.5f;
};
