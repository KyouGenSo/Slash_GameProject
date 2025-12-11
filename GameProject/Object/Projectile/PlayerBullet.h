#pragma once

#include "Projectile.h"
#include "../../../GameProject/Collision/CollisionTypeIdDef.h"
#include <memory>
#include <string>

namespace  Tako
{
    class EmitterManager;
}

class PlayerBulletCollider;

/// <summary>
/// プレイヤーの弾クラス
/// BossBulletと対称の設計パターン
/// </summary>
class PlayerBullet : public Projectile {
    //=========================================================================================
    // 定数
    //=========================================================================================
private:
    static constexpr uint32_t kIdResetThreshold = 10000; ///< ID リセット閾値
    static constexpr float kInitialScale = 0.0f;         ///< 初期スケール（パーティクル描画のため0）

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="emitterManager">エミッターマネージャー</param>
    PlayerBullet(Tako::EmitterManager* emitterManager);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PlayerBullet() override;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="position">初期位置</param>
    /// <param name="velocity">初期速度</param>
    void Initialize(const Tako::Vector3& position, const Tako::Vector3& velocity) override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間</param>
    void Update(float deltaTime) override;

    /// <summary>
    /// コリジョンタイプIDを取得
    /// </summary>
    CollisionTypeId GetTypeId() const { return CollisionTypeId::PLAYER_ATTACK; }

    /// <summary>
    /// コライダーを取得
    /// </summary>
    PlayerBulletCollider* GetCollider() const { return collider_.get(); }

private:
    // モデルを設定
    void SetModel();

private:
    // 専用コライダー
    std::unique_ptr<PlayerBulletCollider> collider_;

    // エミッタマネージャーへのポインタ
    Tako::EmitterManager* emitterManager_ = nullptr;

    // エミッターの名前
    std::string bulletEmitterName_ = "";
    std::string explodeEmitterName_ = "";

    // id（複数弾の識別用）
    static uint32_t id;

    // 調整可能パラメータ
    float yBoundaryMin_ = -10.0f;  ///< Y座標の下限
    float yBoundaryMax_ = 50.0f;   ///< Y座標の上限
};
