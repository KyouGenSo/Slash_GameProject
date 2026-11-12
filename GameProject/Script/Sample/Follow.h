#pragma once
#include "ActorRef.h"
#include "Script.h"

/// <summary>
/// target の Actor へ、止まる距離まで一定の速さで近づく
/// </summary>
TCLASS()
class Follow : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    void Tick(float deltaTime) override;

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "追う相手。Hierarchy の行をここへドロップしても入る")
    Tako::ActorRef target_;
    TPROPERTY(Edit, Tooltip = "1 秒あたりの移動量", ClampMin = 0, Speed = 0.05)
    float          speed_        = 3.0f;
    TPROPERTY(Edit, Tooltip = "相手との距離がこれ以下なら動かない", ClampMin = 0, Speed = 0.05)
    float          stopDistance_ = 1.5f;
};
