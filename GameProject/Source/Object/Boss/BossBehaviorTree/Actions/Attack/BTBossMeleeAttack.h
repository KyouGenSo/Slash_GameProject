#pragma once
#include <numbers>
#include "AttackNode.h"
#include "Vector3.h"

class Boss;

/// <summary>
/// 武器ブロックを振る近接攻撃。コンボ抽選で 1 撃または 3 連撃。
/// </summary>
TCLASS(DisplayName = "Melee Attack", Category = "Action", Color = 0xE6661A)
class BTBossMeleeAttack : public AttackNode {
    GENERATED_BODY()

private: //定数
    static constexpr float kDirectionEpsilon = 0.01f;
    static constexpr float kBlockStartAngle = -std::numbers::pi_v<float> / 2.0f;
    static constexpr float kAngleEpsilon = 0.001f;

private: //構造体
    enum class MeleePhase {
        Prepare,    ///< 準備（プレイヤー方向を向く、予兆表示）
        Execute,    ///< 攻撃実行（ブロック回転、ダメージ判定）
        Interval,   ///< コンボ間隔（次の攻撃までの待機）
        Recovery    ///< 硬直
    };

public: //メンバー関数
    BTBossMeleeAttack();
    virtual ~BTBossMeleeAttack() = default;

    //=====================================
    //Setter
    //=====================================
    void  SetPrepareTime(float time) { prepareTime_ = time; }
    void  SetAttackDuration(float duration) { attackDuration_ = duration; }
    void  SetRecoveryTime(float time) { recoveryTime_ = time; }
    void  SetBlockRadius(float radius) { blockRadius_ = radius; }
    void  SetBlockScale(float scale) { blockScale_ = scale; }
    void  SetSwingAngle(float angle) { swingAngle_ = angle; }
    void  SetRushDistance(float distance) { rushDistance_ = distance; }
    void  SetStopDistance(float distance) { stopDistance_ = distance; }

    //=====================================
    //Getter
    //=====================================
    float GetPrepareTime() const { return prepareTime_; }
    float GetAttackDuration() const { return attackDuration_; }
    float GetRecoveryTime() const { return recoveryTime_; }
    float GetBlockRadius() const { return blockRadius_; }
    float GetBlockScale() const { return blockScale_; }
    float GetSwingAngle() const { return swingAngle_; }
    float GetRushDistance() const { return rushDistance_; }
    float GetStopDistance() const { return stopDistance_; }

protected: //非公開関数
    /// <summary>
    /// Prepare → Execute →（コンボ時 Interval）→ Recovery の状態機械を進行させる
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="boss">攻撃させるボス</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    /// <returns>Recovery 完了で Success、それ以外は Running</returns>
    Tako::BTNodeStatus OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) override;

    void OnInitialize(Tako::BTBlackboard* blackboard, Boss* boss) override;

    void OnCleanup() override;

#ifdef _DEBUG
    bool DrawCustomImGui() override;
#endif

private: //非公開関数
    /// <summary>
    /// プレイヤー方向へ徐々に旋回
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    void AimAtPlayer(Tako::BTBlackboard* blackboard, float deltaTime);

    /// <summary>
    /// 準備フェーズ: プレイヤーを向き、ブロック位置と予兆エミッタを更新する
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    void ProcessPreparePhase(Tako::BTBlackboard* blackboard, float deltaTime);

    /// <summary>
    /// 突進移動とブロック振り回し
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    void ProcessExecutePhase(Tako::BTBlackboard* blackboard, float deltaTime);

    /// <summary>
    /// 硬直フェーズ（現状は何もしない）
    /// </summary>
    /// <param name="boss">対象のボス</param>
    void ProcessRecoveryPhase(Boss* boss);

    /// <summary>
    /// コンボ間隔フェーズ: プレイヤー方向へ旋回し続ける
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    /// <param name="deltaTime">前フレームからの経過秒</param>
    void ProcessIntervalPhase(Tako::BTBlackboard* blackboard, float deltaTime);

    /// <summary>
    /// コンボ段に応じた振り方向と初期角度を設定
    /// </summary>
    void InitializeSwingForCurrentCombo();

    /// <summary>
    /// blockAngle_ に基づき武器ブロックの位置・姿勢を更新
    /// </summary>
    /// <param name="boss">武器ブロックを持つボス</param>
    void UpdateBlockPosition(Boss* boss);

    /// <summary>
    /// 突進の開始/目標位置と方向を算出
    /// </summary>
    /// <param name="blackboard">boss / player ポインタを保持する共有ストレージ</param>
    void InitializeRush(Tako::BTBlackboard* blackboard);

private: //メンバー変数
    //パラメータ
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 3, Speed = 0.05)
    float prepareTime_      = 1.0f;
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 3, Speed = 0.05)
    float attackDuration_   = 0.3f;
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 3, Speed = 0.05)
    float recoveryTime_     = 0.3f;
    TPROPERTY(Edit, ClampMin = 0.5, ClampMax = 10, Speed = 0.1)
    float blockRadius_      = 8.0f;
    TPROPERTY(Edit, ClampMin = 0.1, ClampMax = 5, Speed = 0.1)
    float blockScale_       = 0.5f;
    TPROPERTY(Edit, Angle, ClampMin = 0, ClampMax = 360)
    float swingAngle_       = static_cast<float>(std::numbers::pi);
    TPROPERTY(Edit, ClampMin = 0, ClampMax = 50, Speed = 1)
    float rushDistance_     = 20.0f;
    TPROPERTY(Edit, ClampMin = 1, ClampMax = 20, Speed = 0.5)
    float stopDistance_     = 5.0f;
    TPROPERTY(Edit, Category = "Combo", ClampMin = 0.1, ClampMax = 2, Speed = 0.05)
    float comboInterval_    = 0.5f;
    TPROPERTY(Edit, Category = "Combo", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float comboProbability_ = 0.5f;

    //ランタイム状態
    MeleePhase currentPhase_      = MeleePhase::Prepare;
    float      totalDuration_     = 1.6f;
    float      phaseTimer_        = 0.0f;
    float      blockAngle_        = 0.0f;
    bool       colliderActivated_ = false;

    //突進状態
    Tako::Vector3 startPosition_;
    Tako::Vector3 targetPosition_;
    Tako::Vector3 rushDirection_;
    bool          rushInitialized_ = false;

    //コンボ管理
    bool  isComboMode_           = false;
    int   comboMaxCount_         = 1;
    int   comboIndex_            = 0;
    float currentSwingDirection_ = 1.0f;
};
