#pragma once
#include "Script.h"
#include "Vector3.h"

#include <string_view>

namespace Tako {
    class BaseScene;
}

/// <summary>
/// まっすぐ飛ぶ弾。プレハブ（BossBullet / PenetratingBossBullet / PlayerBullet）に付けて、Bullet::Spawn で撃つ。
/// 当たり判定は同じ Actor の ColliderComponent（層 BOSS_PROJECTILE / PLAYER_PROJECTILE）、
/// 見た目は ParticleSystemComponent。消えるときに "Explode" という名前の ParticleSystemComponent を 1 回だけ出す
/// </summary>
TCLASS()
class Bullet : public Tako::Script {
    GENERATED_BODY()

public: //構造体
    TENUM()
    enum class Target { Player, Boss };

public: //メンバー関数
    /// <summary>
    /// プレハブから弾を作って撃つ。シーンに入って動き出すのは次のフレームから
    /// </summary>
    /// <param name="prefab">Bullet を付けたプレハブの名前</param>
    static void Spawn(Tako::BaseScene& scene, std::string_view prefab, const Tako::Vector3& position, const Tako::Vector3& velocity);

    void Tick(float deltaTime) override;

    void OnCollisionEnter(Tako::Actor* other) override;

private: //非公開関数
    /// <summary>
    /// 消える演出を出して、Actor を消す
    /// </summary>
    void Finish();

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "ダメージを与える相手")
    Target target_               = Target::Player;
    TPROPERTY(Edit, ClampMin = 0, Speed = 0.5)
    float  damage_               = 10.0f;
    TPROPERTY(Edit, Tooltip = "秒。過ぎたら消える", ClampMin = 0, Speed = 0.1)
    float  lifetime_             = 5.0f;
    TPROPERTY(Edit, Tooltip = "相手側の弾（Target が逆の Bullet）に当たったら消える")
    bool   cancelsOnEnemyBullet_ = true;
    TPROPERTY(Edit, Tooltip = "シーンのフォースフィールド（弾に効く物）の力を受ける")
    bool   affectedByForceField_ = false;
    TPROPERTY(Edit, Tooltip = "速さがこれ未満になったら消える（0 で無効。フォースフィールドで止められた弾用）", ClampMin = 0, Speed = 0.05)
    float  minSpeed_             = 0.0f;
    TPROPERTY(Edit, Tooltip = "高さがこれを下回ったら消える", Speed = 0.5)
    float  minY_                 = 0.0f;
    TPROPERTY(Edit, Tooltip = "高さがこれを上回ったら消える", Speed = 0.5)
    float  maxY_                 = 50.0f;

    Tako::Vector3 velocity_{};
    float         elapsedTime_ = 0.0f;
    bool          isFinished_  = false;
};
