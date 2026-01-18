#pragma once

#include "SphereCollider.h"
#include <unordered_set>

class PenetratingBossBullet;
class Player;

/// <summary>
/// 貫通弾専用コライダー
/// プレイヤーの弾と衝突しても消えず、プレイヤー本体に当たった場合のみ消える
/// </summary>
class PenetratingBossBulletCollider : public Tako::SphereCollider {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="owner">所有者（PenetratingBossBullet）</param>
    PenetratingBossBulletCollider(PenetratingBossBullet* owner);

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~PenetratingBossBulletCollider() override = default;

    /// <summary>
    /// 衝突開始時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter(Tako::Collider* other) override;

    /// <summary>
    /// 衝突継続時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay(Tako::Collider* other) override;

    /// <summary>
    /// 衝突終了時の処理
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionExit(Tako::Collider* other) override;

    /// <summary>
    /// リセット
    /// </summary>
    void Reset();

    /// <summary>
    /// ヒットしたプレイヤーを取得
    /// </summary>
    Player* GetHitPlayer() const { return hitPlayer_; }

private:
    // 所有者（PenetratingBossBullet）への参照
    PenetratingBossBullet* owner_ = nullptr;

    // ヒット済みターゲット（多重ヒット防止用）
    std::unordered_set<void*> hitTargets_;

    // 現在ヒットしているプレイヤー
    Player* hitPlayer_ = nullptr;

    // ダメージを与えたかどうか
    bool hasDealtDamage_ = false;
};
