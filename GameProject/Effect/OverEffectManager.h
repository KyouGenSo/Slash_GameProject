#pragma once
#include "TransitionEffectBase.h"
#include "Vector3.h"

class Player;

/// <summary>
/// ゲームオーバー演出管理クラス
/// プレイヤー死亡時のエミッター発火とスケール減少を制御
/// TransitionEffectBaseを継承
/// </summary>
class OverEffectManager : public TransitionEffectBase
{
public:
    /// <summary>
    /// 演出フェーズ
    /// </summary>
    enum class Phase {
        Idle,       ///< 待機中（演出未開始）
        WaitEmit1,  ///< 第1エミッター発火待機
        WaitEmit2,  ///< 第2エミッター発火待機
        ScaleDown,  ///< スケール減少中
        Complete    ///< 演出完了（シーン遷移可能）
    };

    /// <summary>
    /// 演出パラメータ
    /// </summary>
    struct Params {
        float emit1Time = 2.0f;          ///< 第1エミッター発火時間
        float emit2Time = 2.8f;          ///< 第2エミッター発火時間
        float totalTime = 3.8f;          ///< 演出総時間
        float scaleDecreaseRate = 5.0f;  ///< スケール減少速度
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit OverEffectManager(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~OverEffectManager() override = default;

    /// <summary>
    /// 演出開始
    /// </summary>
    void Start() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">フレーム間隔（秒）</param>
    void Update(float deltaTime) override;

    /// <summary>
    /// リセット
    /// </summary>
    void Reset() override;

    /// <summary>
    /// 対象プレイヤーを設定
    /// </summary>
    /// <param name="player">プレイヤーへのポインタ</param>
    void SetTarget(Player* player) { target_ = player; }

    /// <summary>
    /// パラメータを設定
    /// </summary>
    /// <param name="params">パラメータ</param>
    void SetParams(const Params& params) { params_ = params; }

    /// <summary>
    /// パラメータを取得
    /// </summary>
    /// <returns>現在のパラメータ</returns>
    const Params& GetParams() const { return params_; }

    /// <summary>
    /// 現在のフェーズを取得
    /// </summary>
    /// <returns>現在のフェーズ</returns>
    Phase GetPhase() const { return phase_; }

private:
    Player* target_ = nullptr;         ///< 対象プレイヤー
    Phase phase_ = Phase::Idle;        ///< 現在のフェーズ
    bool emit1Fired_ = false;          ///< 第1エミッター発火済みフラグ
    bool emit2Fired_ = false;          ///< 第2エミッター発火済みフラグ
    Params params_;                    ///< パラメータ
};
