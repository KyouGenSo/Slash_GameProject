#pragma once

#include "SphereCollider.h"
#include <unordered_set>

class PlayerBullet;
class Boss;

/// <summary>
/// プレイヤーの弾専用コライダー
/// BossBulletColliderと対称の設計パターン
/// </summary>
class PlayerBulletCollider : public SphereCollider {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="owner">所有者（PlayerBullet）</param>
    PlayerBulletCollider(PlayerBullet* owner);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PlayerBulletCollider() override = default;

    /// <summary>
    /// 衝突開始時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter(Collider* other) override;

    /// <summary>
    /// 衝突継続時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay(Collider* other) override;

    /// <summary>
    /// 衝突終了時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionExit(Collider* other) override;

    /// <summary>
    /// リセット
    /// </summary>
    void Reset();

    /// <summary>
    /// ヒットしたボスを取得
    /// </summary>
    Boss* GetHitBoss() const { return hitBoss_; }

private:
    // 所有者（PlayerBullet）への参照
    PlayerBullet* owner_ = nullptr;

    // ヒット済みターゲット（多重ヒット防止用）
    std::unordered_set<void*> hitTargets_;

    // 現在ヒットしているボス
    Boss* hitBoss_ = nullptr;

    // ダメージを与えたかどうか
    bool hasDealtDamage_ = false;
};
