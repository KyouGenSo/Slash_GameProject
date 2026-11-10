#pragma once
#include "Script.h"
#include "Vector3.h"

namespace Tako {
    class ColliderComponent;
}

class Boss;

/// <summary>
/// プレイヤー近接攻撃の判定と敵へのダメージ処理。プレイヤーの子の Actor に、ColliderComponent（Box、層 PLAYER_ATTACK）と一緒に付ける。
/// 当たり判定は Actor の位置と向きに付いていくので、Transform を動かさなければプレイヤーの前に出る
/// </summary>
TCLASS(Requires = "ColliderComponent")
class MeleeAttackCollider : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    /// <summary>
    /// 同じ Actor の ColliderComponent を取り、攻撃が始まるまで止めておく
    /// </summary>
    void Awake() override;

    /// <summary>
    /// ボス検出時、canDamage_ が立っていればダメージとノックバックを1回適用
    /// </summary>
    /// <param name="other">相手の Actor</param>
    void OnCollisionStay(Tako::Actor* other) override;

    /// <summary>
    /// 検出済みの敵をクリアし、次の攻撃で再検出できるようにする
    /// </summary>
    void Reset();

    /// <summary>
    /// canDamage_ フラグを立てるだけ。実ダメージは次の OnCollisionStay のヒット時に適用される
    /// </summary>
    void Damage();

    //====================================
    //Setter
    //====================================
    void SetActive(bool active);

    /// <summary>
    /// 箱の全長
    /// </summary>
    void SetSize(const Tako::Vector3& size);

    /// <summary>
    /// Actor からのずれ（Actor の向きで回る）
    /// </summary>
    void SetOffset(const Tako::Vector3& offset);

    /// <summary>
    /// ノックバック有効フラグを設定（4コンボ目のみ true）
    /// </summary>
    void SetKnockbackEnabled(bool enabled) { knockbackEnabled_ = enabled; }

    void SetAttackDamage(float damage) { attackDamage_ = damage; }

    //====================================
    //Getter
    //====================================
    bool IsActive() const;

    Boss* GetDetectedEnemy() const { return detectedEnemy_; }

#ifdef _DEBUG
    int GetCollisionCount() const { return collisionCount_; }
#endif

private: //メンバー変数
    Tako::ColliderComponent* collider_         = nullptr;  ///< 同じ Actor の物
    Boss*                    detectedEnemy_    = nullptr;
    bool                     canDamage_        = false;
    float                    attackDamage_{};
    bool                     knockbackEnabled_ = true;     ///< 4コンボ目のみ true

#ifdef _DEBUG
    int collisionCount_ = 0;
#endif
};
