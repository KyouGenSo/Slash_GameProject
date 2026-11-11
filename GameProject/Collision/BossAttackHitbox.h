#pragma once
#include "ActorRef.h"
#include "Script.h"
#include "Transform.h"
#include "Vector3.h"

#include <string>

namespace Tako {
    class BaseScene;
}

class Boss;

/// <summary>
/// ボスの攻撃が、攻撃の間だけ出す当たり判定。ColliderComponent（層 BOSS_ATTACK）と一緒にプレハブ（BossAttackSphere / BossAttackBox）に付ける。
/// 位置・大きさ・ダメージ・当たりの有無は、出した BT ノードが決める
/// </summary>
TCLASS(Requires = "ColliderComponent")
class BossAttackHitbox : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    /// <summary>
    /// プレイヤーに当たったら、パリィ中なら成功扱い、違えば damage_ を与える
    /// </summary>
    /// <param name="other">相手の Actor</param>
    void OnCollisionEnter(Tako::Actor* other) override;

    //==========================
    //Setter
    //==========================
    /// <summary>
    /// 同じ Actor の ColliderComponent の当たりを有効・無効にする
    /// </summary>
    void SetActive(bool active);

    /// <summary>
    /// ワールドでの位置と向き（親の無い Actor として出すので、Actor の Transform にそのまま入れる）
    /// </summary>
    void SetTransform(const Tako::Transform& transform);

    /// <summary>
    /// 球の半径
    /// </summary>
    void SetRadius(float radius);

    /// <summary>
    /// 箱の全長
    /// </summary>
    void SetSize(const Tako::Vector3& size);

    void SetDamage(float damage) { damage_ = damage; }

    /// <summary>
    /// 複数の当たり判定で 1 発だけ通す共有ガードを設定（nullptr で個別ヒットに戻る）
    /// </summary>
    void SetSharedHitGuard(bool* guard) { sharedHitGuard_ = guard; }

    //==========================
    //Getter
    //==========================
    float GetDamage() const { return damage_; }

private: //メンバー変数
    float damage_         = 15.0f;
    bool* sharedHitGuard_ = nullptr;  ///< 非所有。グループ全体で共有するヒット済みフラグ
};

/// <summary>
/// BT ノードが持つ、BossAttackHitbox の Actor への参照。ID で指すので、Actor が（エディタなどで）先に消されても壊れない。
/// 指す相手が居なければ false になる（-> の前に確かめる）
/// </summary>
class BossAttackHitboxRef {
public: //メンバー関数
    /// <summary>
    /// プレハブから当たり判定の Actor を出し、当たりを止めた状態で指す。前に指していた物は消す。シーンに入るのは次のフレームの頭
    /// </summary>
    /// <param name="prefab">BossAttackHitbox を付けたプレハブの名前</param>
    /// <param name="transform">ワールドでの位置と向き</param>
    void Spawn(Boss* boss, const std::string& prefab, const Tako::Transform& transform);

    /// <summary>
    /// 指している Actor を消す（次のフレームの頭）。それまでに当たらないよう、先に当たりを止める
    /// </summary>
    void Destroy();

    //==========================
    //Getter
    //==========================
    /// <summary>
    /// 指している当たり判定。出していないか、もう無ければ nullptr
    /// </summary>
    BossAttackHitbox* Get() const;

    explicit operator bool() const { return Get() != nullptr; }

    BossAttackHitbox* operator->() const { return Get(); }

private: //メンバー変数
    Tako::BaseScene* scene_ = nullptr;
    Tako::ActorId    id_    = 0;
};
