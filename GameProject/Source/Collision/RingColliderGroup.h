#pragma once
#include "Script/Collision/BossAttackHitbox.h"
#include "Transform.h"
#include "Vector3.h"
#include <array>
#include <string>

class Boss;

/// <summary>
/// 円弧上に球の当たり判定（プレハブから出した Actor）を並べて、リング状衝撃波の当たり判定を近似するグループ
/// </summary>
class RingColliderGroup {
public: //定数
    static constexpr int kMaxSegments = 16;
    static constexpr int kMinSegments = 3;

private: //定数
    static constexpr float kOverlapFactor = 1.15f;  ///< 隣接球の隙間を埋める重複率

public: //メンバー関数
    RingColliderGroup() = default;
    ~RingColliderGroup();

    /// <summary>
    /// セグメント数分の当たり判定を、当たりを止めた状態でシーンに出す
    /// </summary>
    /// <param name="boss">ダメージ元のボス</param>
    /// <param name="prefab">BossAttackHitbox を付けた球のプレハブ</param>
    /// <param name="segmentCount">弧上の球数（kMinSegments〜kMaxSegments に clamp）</param>
    void Initialize(Boss* boss, const std::string& prefab, int segmentCount);

    /// <summary>
    /// 全部の当たり判定をシーンから消す（二重呼び出し安全）
    /// </summary>
    void Finalize();

    /// <summary>
    /// ヒットガードとスイープ帯の基準半径をリセットして全セグメントを有効化する
    /// </summary>
    /// <param name="midRadius">現在の帯中心半径</param>
    void Activate(float midRadius);

    void Deactivate();

    /// <summary>
    /// 円弧上へ全セグメントを再配置する。前フレーム半径との差を球半径に
    /// 含めるスイープ帯方式で、高速拡大時のすり抜けを防ぐ
    /// </summary>
    /// <param name="center">弧の中心。y がそのまま球中心の高さになる</param>
    /// <param name="yawRad">弧の中心方向（+Z 基準、atan2(x, z) 系）</param>
    /// <param name="sweepRad">弧の全角（半円 = π）</param>
    /// <param name="midRadius">帯中心の半径</param>
    /// <param name="ringWidth">帯の半径方向の厚み</param>
    /// <param name="extraBand">追加マージン（1 フレームの前進量など）</param>
    void UpdateArc(const Tako::Vector3& center, float yawRad, float sweepRad,
                   float midRadius, float ringWidth, float extraBand);

    //===========================
    //Setter
    //===========================
    void SetDamage(float damage);

    //===========================
    //Getter
    //===========================
    bool HasHitPlayer() const { return hitGuard_; }

private: //メンバー変数
    //コライダー
    std::array<BossAttackHitboxRef, kMaxSegments> colliders_{};
    std::array<Tako::Transform, kMaxSegments>     transforms_{};

    //配置状態
    int   segmentCount_  = kMaxSegments;
    float prevMidRadius_ = 0.0f;
    bool  hitGuard_      = false;  ///< 全セグメント共有の 1 発ガード
};
