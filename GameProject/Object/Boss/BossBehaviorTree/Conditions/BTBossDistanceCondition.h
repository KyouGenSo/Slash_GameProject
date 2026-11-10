#pragma once
#include "BTNode.h"
#include "BTBlackboard.h"

/// <summary>
/// プレイヤーとの水平距離が指定範囲内かを判定する条件ノード
/// </summary>
TCLASS(DisplayName = "Distance Condition", Category = "Condition", Color = 0x33B280)
class BTBossDistanceCondition : public Tako::BTNode {
    GENERATED_BODY()

public: //メンバー関数
    BTBossDistanceCondition();
    virtual ~BTBossDistanceCondition() = default;

    /// <summary>
    /// プレイヤーとの水平距離（Y 無視）が [min, max] の範囲内かを判定する
    /// </summary>
    /// <param name="blackboard">"boss" と "player" ポインタを保持するブラックボード</param>
    /// <returns>範囲内なら Success。範囲外または boss/player が未設定なら Failure</returns>
    Tako::BTNodeStatus Execute(Tako::BTBlackboard* blackboard) override;
    void Reset() override;
    void PostEditChange() override;
#ifdef _DEBUG
    bool DrawCustomImGui() override;
#endif

    //=============================
    //Setter
    //=============================
    void SetMinDistance(float dist) { minDistance_ = dist; }
    void SetMaxDistance(float dist) { maxDistance_ = dist; }

    //=============================
    //Getter
    //=============================
    float GetMinDistance() const { return minDistance_; }
    float GetMaxDistance() const { return maxDistance_; }

private: //メンバー変数
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 100, Speed = 0.5)
    float minDistance_ = 0.0f;
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 100, Speed = 0.5)
    float maxDistance_ = 15.0f;
};
