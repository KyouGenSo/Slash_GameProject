#pragma once
#include "Component.h"
#include "Vector3.h"

/// <summary>
/// BeginPlay の位置から offset まで行って戻るのを繰り返す
/// </summary>
TCLASS()
class Mover : public Tako::Component {
    GENERATED_BODY()

public: //メンバー関数
    void BeginPlay() override;

    void Tick(float deltaTime) override;

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "BeginPlay の位置から一番遠くまでの移動量", Speed = 0.05)
    Tako::Vector3 offset_{ 0.0f, 0.0f, 5.0f };
    TPROPERTY(Edit, Tooltip = "往復 1 回の秒数", ClampMin = 0.01, Speed = 0.05)
    float period_ = 2.0f;

    Tako::Vector3 origin_{};
    float         time_ = 0.0f;
};
