#pragma once
#include "Reflection.h"
#include "Vector3.h"

#include <numbers>

/// <summary>
/// 肩越しのカメラ（ThirdPersonController）の調整値
/// </summary>
TSTRUCT()
struct ThirdPersonCameraSettings {
    GENERATED_STRUCT_BODY()

    TPROPERTY(Edit, Tooltip = "追う相手からのずれ", Speed = 0.1)
    Tako::Vector3 offset                  = { 0.0f, 2.0f, -40.0f };
    TPROPERTY(Edit, Angle, Tooltip = "見下ろす角度")
    float         angleX                  = 8.0f * std::numbers::pi_v<float> / 180.0f;
    TPROPERTY(Edit, Angle, Tooltip = "注視する相手（ボス）を向くときに足す見下ろしの角度")
    float         lookDownAngle           = 15.0f * std::numbers::pi_v<float> / 180.0f;
    TPROPERTY(Edit, Tooltip = "右スティックで回す速さ（度 / フレーム）", ClampMin = 0, Speed = 0.005)
    float         rotateSpeed             = 0.05f;
    TPROPERTY(Edit, Tooltip = "ゲームパッドで回すときに掛ける倍率", ClampMin = 0, Speed = 0.05)
    float         gamepadRotateMultiplier = 1.0f;
    TPROPERTY(Edit, Tooltip = "大きいほどすぐ追いつく", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float         followSmoothness        = 0.18f;
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float         offsetLerpSpeed         = 0.08f;
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float         rotationLerpSpeed       = 0.15f;
    TPROPERTY(Edit, Angle, DisplayName = "FOV")
    float         fov                     = 0.45f;
};

/// <summary>
/// 見下ろしのカメラ（TopDownController）の調整値。高さと後ろへの引きは、追う相手どうしの距離に倍率を掛けて決める
/// </summary>
TSTRUCT()
struct TopDownCameraSettings {
    GENERATED_STRUCT_BODY()

    TPROPERTY(Edit, Category = "Height", Speed = 0.5)
    float baseHeight       = 10.0f;
    TPROPERTY(Edit, Category = "Height", Tooltip = "相手どうしの距離に掛ける倍率", Speed = 0.01)
    float heightMultiplier = 0.6f;
    TPROPERTY(Edit, Category = "Height", Speed = 0.5)
    float minHeight        = 26.0f;
    TPROPERTY(Edit, Category = "Height", Speed = 0.5)
    float maxHeight        = 500.0f;
    TPROPERTY(Edit, Category = "Height", Tooltip = "始まったときの高さ", Speed = 0.5)
    float initialHeight    = 88.0f;

    TPROPERTY(Edit, Category = "Back Offset", Speed = 0.5)
    float baseBackOffset       = -10.0f;
    TPROPERTY(Edit, Category = "Back Offset", Tooltip = "相手どうしの距離に掛ける倍率", Speed = 0.01)
    float backOffsetMultiplier = 1.5f;
    TPROPERTY(Edit, Category = "Back Offset", Speed = 0.5)
    float minBackOffset        = -500.0f;
    TPROPERTY(Edit, Category = "Back Offset", Speed = 0.5)
    float maxBackOffset        = -52.0f;
    TPROPERTY(Edit, Category = "Back Offset", Tooltip = "始まったときの引き", Speed = 0.5)
    float initialBackOffset    = -205.0f;

    TPROPERTY(Edit, Category = "View", Angle, Tooltip = "見下ろす角度")
    float angleX           = 25.0f * std::numbers::pi_v<float> / 180.0f;
    TPROPERTY(Edit, Category = "View", Tooltip = "大きいほどすぐ追いつく", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float followSmoothness = 0.18f;
    TPROPERTY(Edit, Category = "View", Angle, DisplayName = "FOV")
    float fov              = 0.45f;
};
