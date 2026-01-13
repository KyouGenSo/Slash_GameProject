#pragma once
#include "Vector3.h"

namespace Tako {
class EmitterManager;
}

/// <summary>
/// ボスフェーズ2境界線パーティクル管理クラス
/// 戦闘エリアの境界線を表示するエミッター4個を制御
/// </summary>
class BossBorderParticleManager
{
public:
    /// <summary>
    /// パラメータ構造体
    /// </summary>
    struct Params {
        float areaSize = 20.0f;  ///< 戦闘エリアサイズ
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit BossBorderParticleManager(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~BossBorderParticleManager() = default;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="bossPhase">ボスの現在フェーズ</param>
    /// <param name="bossPosition">ボスの位置</param>
    void Update(int bossPhase, const Tako::Vector3& bossPosition);

    /// <summary>
    /// 境界線エミッターの有効/無効を設定
    /// </summary>
    /// <param name="active">有効にするならtrue</param>
    void SetActive(bool active);

    /// <summary>
    /// 境界線エミッターがアクティブか
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
    /// <summary>
    /// エミッター位置をボス位置に追従させる
    /// </summary>
    /// <param name="bossPosition">ボスの位置</param>
    void UpdatePositions(const Tako::Vector3& bossPosition);

    Tako::EmitterManager* emitterManager_ = nullptr;  ///< エミッターマネージャー
    bool isActive_ = false;                           ///< エミッターアクティブ状態
    Params params_;                                   ///< パラメータ
};
