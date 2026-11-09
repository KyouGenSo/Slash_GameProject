#pragma once
#include "BTNode.h"
#include "BTBlackboard.h"

class Boss;

/// <summary>
/// プレイヤーを向きつつ idleDuration_ だけ待機する
/// </summary>
TCLASS()
class BTBossIdle : public Tako::BTNode {
    GENERATED_BODY()

private: //定数
    static constexpr float kDirectionEpsilon = 0.01f;

public: //メンバー関数
    BTBossIdle();
    virtual ~BTBossIdle() = default;

    /// <summary>
    /// プレイヤーを向きつつ idleDuration_ だけ待機する
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <returns>boss 未取得で Failure、idleDuration_ 経過で Success、待機中は Running</returns>
    Tako::BTNodeStatus Execute(Tako::BTBlackboard* blackboard) override;

    void Reset() override;

    //================================
    //Setter
    //================================
    void SetIdleDuration(float duration) { idleDuration_ = duration; }

    //================================
    //Getter
    //================================
    float GetIdleDuration() const { return idleDuration_; }

private: //非公開関数
    /// <summary>
    /// プレイヤー方向へ rotationSpeed_ で旋回する（1フレームの回転量を制限）
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    void LookAtPlayer(Tako::BTBlackboard* blackboard, float deltaTime);

private: //メンバー変数
    //パラメータ
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 10, Speed = 0.1)
    float idleDuration_  = 2.0f;
    TPROPERTY(Edit, ClampMin = 0.1, ClampMax = 20, Speed = 0.1)
    float rotationSpeed_ = 5.0f;   ///< ラジアン/秒

    //ランタイム状態
    float elapsedTime_    = 0.0f;
    bool  isFirstExecute_ = true;
};
