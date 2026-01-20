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
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー（依存注入）</param>
    explicit BossBorderParticleManager(Tako::EmitterManager* emitterManager, float areaSize);

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
    /// 戦闘エリアサイズを設定
    /// </summary>
    /// <param name="params">パラメータ</param>
    void SetAreaSize(float areaSize) { areaSize_ = areaSize; }

    /// <summary>
    /// 戦闘エリアサイズを取得
    /// </summary>
    /// <returns>現在のパラメータ</returns>
    float GetAreaSize() const { return areaSize_; }

private:
    /// <summary>
    /// エミッター位置をボス位置に追従させる
    /// </summary>
    /// <param name="bossPosition">ボスの位置</param>
    void UpdatePositions(const Tako::Vector3& bossPosition);

private:
    Tako::EmitterManager* emitterManager_ = nullptr;  ///< エミッターマネージャー
    bool isActive_ = false;                           ///< エミッターアクティブ状態
    float areaSize_ = 0.0f;                           ///< 戦闘エリアサイズ
};
