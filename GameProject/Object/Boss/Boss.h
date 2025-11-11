#pragma once
#include <memory>

#include "Transform.h"
#include "Vector4.h"

class OBBCollider;
class Object3d;

/// <summary>
/// ボスエネミークラス
/// HPとフェーズ管理、ダメージ処理を制御
/// </summary>
class Boss
{
public:
    Boss();
    ~Boss();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damage">受けるダメージ量</param>
    void OnHit(float damage);

    /// <summary>
    /// ダメージされるとき色変わる演出
    /// </summary>
    /// <param name="color">変化後の色</param>
    /// <param name="duration">変化時間</param>
    void UpdateHitEffect(Vector4 color, float duration);

    //-----------------------------Getters/Setters------------------------------//
    /// <summary>
    /// 座標変換情報を設定
    /// </summary>
    /// <param name="transform">新しい座標変換情報</param>
    void SetTransform(const Transform& transform) { transform_ = transform; }

    /// <summary>
    /// HPを設定
    /// </summary>
    /// <param name="hp">新しいHP値</param>
    void SetHp(float hp) { hp_ = hp; }

    /// <summary>
    /// フェーズを設定
    /// </summary>
    /// <param name="phase">新しいフェーズ番号</param>
    void SetPhase(uint32_t phase) { phase_ = phase; }

    /// <summary>
    /// 座標変換情報を取得
    /// </summary>
    /// <returns>現在の座標変換情報の参照</returns>
    const Transform& GetTransform() const { return transform_; }

    /// <summary>
    /// 座標変換情報のポインタを取得
    /// </summary>
    /// <returns>座標変換情報への非constポインタ</returns>
    Transform* GetTransformPtr() { return &transform_; }

    /// <summary>
    /// HPを取得
    /// </summary>
    /// <returns>現在のHP値</returns>
    float GetHp() const { return hp_; }

    /// <summary>
    /// 現在のフェーズを取得
    /// </summary>
    /// <returns>フェーズ番号</returns>
    uint32_t GetPhase() const { return phase_; }

    /// <summary>
    /// ユニークIDを取得
    /// </summary>
    /// <returns>ボスのユニークID</returns>
    uint32_t GetID() const { return id_; }

    /// <summary>
    /// コライダーを取得
    /// </summary>
    /// <returns>ボスのAABBコライダーのポインタ</returns>
    OBBCollider* GetCollider() const { return bodyCollider_.get(); }

private:
    /// ボスの3Dモデルオブジェクト（描画とアニメーション管理）
    std::unique_ptr<Object3d> model_;

    /// ボスの座標変換情報（位置、回転、スケール）
    Transform transform_{};

    /// ボスの現在HP（0になると撃破、初期値200）
    float hp_ = 200.0f;

    /// 現在の戦闘フェーズ（1から開始、フェーズごとに行動パターンが変化）
    uint32_t phase_ = 1;

    /// 最大フェーズ数（5フェーズまで存在、HP減少で遷移）
    uint32_t maxPhase_ = 5;

    /// ボス本体の衝突判定用AABBコライダー（矩形境界ボックス）
    std::unique_ptr<OBBCollider> bodyCollider_;

    /// このボスインスタンス固有のユニークID
    uint32_t id_;

    /// ヒットエフェクトの再生状態を示すフラグ。
    bool isPlayHitEffect_ = false;

    /// ヒットエフェクトのタイマー
    float hitEffectTimer_ = 0.0f;

};

