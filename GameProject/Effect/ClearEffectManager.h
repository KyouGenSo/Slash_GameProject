#pragma once
#include "TransitionEffectBase.h"
#include "Vector3.h"
#include <cstdint>

class Boss;

/// <summary>
/// ゲームクリア演出管理クラス
/// ボス撃破時の斬撃エフェクト増加と消滅演出を制御
/// TransitionEffectBase を継承
/// </summary>
class ClearEffectManager : public TransitionEffectBase
{
public:
    /// <summary>
    /// 演出フェーズ
    /// </summary>
    enum class Phase {
        Idle,           ///< 待機中（演出未開始）
        SlashBuildup,   ///< 斬撃エフェクト増加中
        Explosion,      ///< 爆発エフェクト
        ScaleDown,      ///< スケール減少中
        Complete        ///< 演出完了（シーン遷移可能）
    };

    /// <summary>
    /// 演出パラメータ
    /// </summary>
    struct Params {
        float startDelay = 0.5f;                 ///< 演出開始までの遅延
        uint32_t slashMaxCount = 100;            ///< 斬撃エミッター最大数
        float slashMaxRadius = 10.0f;            ///< 斬撃エミッター最大半径
        uint32_t slashCountIncrement = 1;        ///< 斬撃数増加量
        float slashRadiusIncrement = 0.05f;      ///< 斬撃半径増加量
        float scaleDecreaseRate = 5.0f;          ///< スケール減少速度
        float shakeDuration = 0.4f;              ///< シェイク持続時間
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit ClearEffectManager(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~ClearEffectManager() override = default;

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
    /// 対象ボスを設定
    /// </summary>
    /// <param name="boss">ボスへのポインタ</param>
    void SetTarget(Boss* boss) { target_ = boss; }

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
    Boss* target_ = nullptr;                     ///< 対象ボス
    Phase phase_ = Phase::Idle;                  ///< 現在のフェーズ
    uint32_t currentSlashCount_ = 1;             ///< 現在の斬撃発生数
    float currentSlashRadius_ = 2.0f;            ///< 現在の斬撃発生半径
    bool explosionFired_ = false;                ///< 爆発エミッター発火済みフラグ
    Params params_;                              ///< パラメータ
};
