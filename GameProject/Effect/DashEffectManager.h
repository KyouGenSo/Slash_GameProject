#pragma once
#include "Vector3.h"
#include <string>

namespace Tako {
class EmitterManager;
}

/// <summary>
/// ダッシュエフェクト管理クラス
/// プレイヤーダッシュ時のエミッター位置をLerp補間で追従させる
/// </summary>
class DashEffectManager
{
public:
    /// <summary>
    /// パラメータ構造体
    /// </summary>
    struct Params {
        float lerpSpeed = 35.0f;                        ///< 補間速度
        float stopThreshold = 0.65f;                    ///< 停止判定閾値
        std::string emitterName = "player_dash";        ///< エミッター名
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit DashEffectManager(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~DashEffectManager() = default;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">フレーム間隔（秒）</param>
    /// <param name="playerPosition">プレイヤーの現在位置</param>
    /// <param name="isDashing">現在ダッシュ中かどうか</param>
    void Update(float deltaTime, const Tako::Vector3& playerPosition, bool isDashing);

    /// <summary>
    /// エミッター位置を初期化
    /// </summary>
    /// <param name="position">初期位置</param>
    void InitializePosition(const Tako::Vector3& position);

    /// <summary>
    /// エフェクトがアクティブか
    /// </summary>
    /// <returns>アクティブならtrue</returns>
    bool IsActive() const { return isActive_; }

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

private:
    Tako::EmitterManager* emitterManager_ = nullptr;   ///< エミッターマネージャー
    Tako::Vector3 emitterPosition_{};                  ///< エミッターの補間位置
    bool previousIsDashing_ = false;                   ///< 前フレームのダッシュ状態
    bool isActive_ = false;                            ///< エミッターのアクティブ状態
    Params params_;                                    ///< パラメータ
};
