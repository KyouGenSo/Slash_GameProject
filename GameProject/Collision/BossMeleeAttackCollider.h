#pragma once
#include "Script.h"
#include "Vector3.h"

namespace Tako {
    class ColliderComponent;
}

/// <summary>
/// ボス近接攻撃のダメージ判定。ボスの子の Actor に、ColliderComponent（Box、層 BOSS_ATTACK）と一緒に付ける。
/// 当たり判定は Actor の位置と向きに付いていくので、Transform を動かさなければボスの前に出る
/// </summary>
TCLASS(Requires = "ColliderComponent")
class BossMeleeAttackCollider : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    /// <summary>
    /// 同じ Actor の ColliderComponent を取り、攻撃が始まるまで止めておく
    /// </summary>
    void Awake() override;

    /// <summary>
    /// プレイヤー初回命中時のみ、パリィ中なら成功扱い、否なら damage_ を適用
    /// </summary>
    /// <param name="other">相手の Actor</param>
    void OnCollisionEnter(Tako::Actor* other) override;

    /// <summary>
    /// 多重ヒット防止フラグをクリアし、次の攻撃で再びヒット可能にする
    /// </summary>
    void Reset();

    //==========================
    //Setter
    //==========================
    void SetActive(bool active);

    /// <summary>
    /// 箱の全長
    /// </summary>
    void SetSize(const Tako::Vector3& size);

    /// <summary>
    /// Actor からのずれ（Actor の向きで回る）
    /// </summary>
    void SetOffset(const Tako::Vector3& offset);

    void SetDamage(float damage) { damage_ = damage; }

    //==========================
    //Getter
    //==========================
    float GetDamage() const { return damage_; }

    bool HasHitPlayer() const { return hasHitPlayer_; }

private: //メンバー変数
    Tako::ColliderComponent* collider_     = nullptr;  ///< 同じ Actor の物
    float                    damage_       = 10.0f;
    bool                     hasHitPlayer_ = false;    ///< 多重ヒット防止
};
