#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"
#include "../../../../Effect/BulletSignEffect.h"
#include "Vector3.h"

class Boss;

/// <summary>
/// ボスの弾幕攻撃アクションノード
/// ステージ中央に移動し、周囲にランダムな方向でランダムな弾を一定時間撃ちまくる
/// 通常弾（速い）と貫通弾（遅い）を混ぜて発射
/// </summary>
class BTBossBarrage : public BTNode {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTBossBarrage();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTBossBarrage() = default;

    /// <summary>
    /// ノードの実行
    /// </summary>
    /// <param name="blackboard">ブラックボード</param>
    /// <returns>実行結果</returns>
    BTNodeStatus Execute(BTBlackboard* blackboard) override;

    /// <summary>
    /// ノードのリセット
    /// </summary>
    void Reset() override;

    // パラメータ取得・設定
    float GetMoveDuration() const { return moveDuration_; }
    void SetMoveDuration(float time) { moveDuration_ = time; }
    float GetChargeTime() const { return chargeTime_; }
    void SetChargeTime(float time) { chargeTime_ = time; }
    float GetFiringDuration() const { return firingDuration_; }
    void SetFiringDuration(float duration) { firingDuration_ = duration; }
    float GetRecoveryTime() const { return recoveryTime_; }
    void SetRecoveryTime(float time) { recoveryTime_ = time; }
    float GetFireInterval() const { return fireInterval_; }
    void SetFireInterval(float interval) { fireInterval_ = interval; }
    float GetNormalBulletSpeedMin() const { return normalBulletSpeedMin_; }
    void SetNormalBulletSpeedMin(float speed) { normalBulletSpeedMin_ = speed; }
    float GetNormalBulletSpeedMax() const { return normalBulletSpeedMax_; }
    void SetNormalBulletSpeedMax(float speed) { normalBulletSpeedMax_ = speed; }
    float GetPenetratingBulletSpeedMin() const { return penetratingBulletSpeedMin_; }
    void SetPenetratingBulletSpeedMin(float speed) { penetratingBulletSpeedMin_ = speed; }
    float GetPenetratingBulletSpeedMax() const { return penetratingBulletSpeedMax_; }
    void SetPenetratingBulletSpeedMax(float speed) { penetratingBulletSpeedMax_ = speed; }
    float GetPenetratingRatio() const { return penetratingRatio_; }
    void SetPenetratingRatio(float ratio) { penetratingRatio_ = ratio; }

    /// <summary>
    /// JSONからパラメータを適用
    /// </summary>
    /// <param name="params">パラメータJSON</param>
    void ApplyParameters(const nlohmann::json& params) override {
        if (params.contains("moveDuration")) {
            moveDuration_ = params["moveDuration"];
        }
        if (params.contains("chargeTime")) {
            chargeTime_ = params["chargeTime"];
        }
        if (params.contains("firingDuration")) {
            firingDuration_ = params["firingDuration"];
        }
        if (params.contains("recoveryTime")) {
            recoveryTime_ = params["recoveryTime"];
        }
        if (params.contains("fireInterval")) {
            fireInterval_ = params["fireInterval"];
        }
        if (params.contains("normalBulletSpeedMin")) {
            normalBulletSpeedMin_ = params["normalBulletSpeedMin"];
        }
        if (params.contains("normalBulletSpeedMax")) {
            normalBulletSpeedMax_ = params["normalBulletSpeedMax"];
        }
        if (params.contains("penetratingBulletSpeedMin")) {
            penetratingBulletSpeedMin_ = params["penetratingBulletSpeedMin"];
        }
        if (params.contains("penetratingBulletSpeedMax")) {
            penetratingBulletSpeedMax_ = params["penetratingBulletSpeedMax"];
        }
        if (params.contains("penetratingRatio")) {
            penetratingRatio_ = params["penetratingRatio"];
        }
    }

    /// <summary>
    /// パラメータをJSONとして抽出
    /// </summary>
    nlohmann::json ExtractParameters() const override;

#ifdef _DEBUG
    /// <summary>
    /// ImGuiでパラメータ編集UIを描画
    /// </summary>
    bool DrawImGui() override;
#endif

private:
    // 定数
    static constexpr float kDirectionEpsilon = 0.001f;  ///< 方向計算の閾値
    static constexpr float kEasingCoeffA = 3.0f;        ///< イージング係数A
    static constexpr float kEasingCoeffB = 2.0f;        ///< イージング係数B

    /// <summary>
    /// 弾幕パラメータの初期化
    /// </summary>
    /// <param name="boss">ボス</param>
    void InitializeBarrage(Boss* boss);

    /// <summary>
    /// 移動フェーズの更新
    /// </summary>
    /// <param name="boss">ボス</param>
    /// <param name="deltaTime">経過時間</param>
    void UpdateMove(Boss* boss, float deltaTime);

    /// <summary>
    /// ランダムな弾を1発発射
    /// </summary>
    /// <param name="boss">ボス</param>
    void FireRandomBullet(Boss* boss);

    // === 時間制御 ===
    float moveDuration_ = 0.5f;           ///< 移動時間
    float chargeTime_ = 0.8f;             ///< チャージ時間
    float firingDuration_ = 3.0f;         ///< 発射時間
    float recoveryTime_ = 0.5f;           ///< 硬直時間
    float fireInterval_ = 0.08f;          ///< 発射間隔

    // === 通常弾速度 ===
    float normalBulletSpeedMin_ = 20.0f;  ///< 通常弾最低速度
    float normalBulletSpeedMax_ = 35.0f;  ///< 通常弾最高速度

    // === 貫通弾速度 ===
    float penetratingBulletSpeedMin_ = 10.0f;   ///< 貫通弾最低速度
    float penetratingBulletSpeedMax_ = 20.0f;   ///< 貫通弾最高速度

    // === 弾種制御 ===
    float penetratingRatio_ = 0.3f;       ///< 貫通弾の割合（0.0〜1.0）

    // === 状態管理 ===
    float totalDuration_ = 0.0f;          ///< 状態の総時間
    float elapsedTime_ = 0.0f;            ///< 経過時間
    float timeSinceLastFire_ = 0.0f;      ///< 前回発射からの経過時間
    bool isFirstExecute_ = true;          ///< 初回実行フラグ
    bool hasEndedEffect_ = false;         ///< エフェクト終了フラグ
    bool enteredRecovery_ = false;        ///< 硬直開始フラグ

    // === 移動制御 ===
    Tako::Vector3 startPosition_;         ///< 移動開始位置
    Tako::Vector3 targetPosition_;        ///< 移動目標位置（ステージ中央）

    // === エフェクト ===
    BulletSignEffect bulletSignEffect_;   ///< 射撃予兆エフェクト
};
