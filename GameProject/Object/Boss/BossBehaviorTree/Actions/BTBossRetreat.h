#pragma once
#include "BTNode.h"
#include "BTBlackboard.h"
#include "../../Movement/BossRetreatExecutor.h"

class Boss;

/// <summary>
/// プレイヤーを向いたまま後方へイージング移動で離脱。移動本体は BossRetreatExecutor へ委譲。
/// </summary>
TCLASS(DisplayName = "Retreat", Category = "Action", Color = 0x4CB2E6)
class BTBossRetreat : public Tako::BTNode {
    GENERATED_BODY()

public: //メンバー関数
    BTBossRetreat();
    virtual ~BTBossRetreat() = default;

    /// <summary>
    /// プレイヤーから離れる方向へ移動。
    /// </summary>
    /// <returns>ボス/プレイヤー不在で Failure、目標距離到達で Success、移動中は Running</returns>
    Tako::BTNodeStatus Execute(Tako::BTBlackboard* blackboard) override;
    void Reset() override;

#ifdef _DEBUG
    bool DrawCustomImGui() override;
#endif

    //====================================
    //Setter
    //====================================
    void SetRetreatSpeed(float speed) { retreatSpeed_ = speed; }
    void SetTargetDistance(float distance) { targetDistance_ = distance; }

    //====================================
    //Getter
    //====================================
    float GetRetreatSpeed()   const { return retreatSpeed_; }
    float GetTargetDistance() const { return targetDistance_; }

private: //メンバー変数
    TPROPERTY(Edit, ClampMin = 10, ClampMax = 200, Speed = 1)
    float retreatSpeed_   = BossRetreatExecutor::Parameters{}.retreatSpeed;
    TPROPERTY(Edit, Tooltip = "プレイヤーからの目標距離", ClampMin = 1, ClampMax = 100, Speed = 0.5)
    float targetDistance_ = BossRetreatExecutor::Parameters{}.targetDistance;

    Boss*               boss_           = nullptr;
    BossRetreatExecutor executor_{};
    bool                isFirstExecute_ = true;
};
