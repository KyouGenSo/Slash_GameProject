#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Transform.h"
#include "vector2.h"
#include "Vector4.h"
#include "Vector3.h"

// 共通定義
#include "../../Common/BulletSpawnRequest.h"
#include "../../Common/ShakeEffect.h"
#include "../../Common/HitFlashEffect.h"
#include "../../Common/BulletSpawner.h"
#include "../../UI/HPBarUI.h"
#include "BossPhaseManager.h"

// Tako namespace前方宣言
namespace Tako {
class OBBCollider;
class Object3d;
class EmitterManager;
}

// GameProject前方宣言
class BossStateMachine;
class BossBehaviorTree;
class BossNodeEditor;
class Player;
class BossShootState;
class BossMeleeAttackCollider;

/// <summary>
/// ボスエネミークラス
/// HPとフェーズ管理、ダメージ処理を制御
/// </summary>
class Boss
{
    // 定数
private:
    // 最大HP
    static constexpr float kMaxHp = 200.0f;

    // フェーズ2開始HP閾値
    static constexpr float kPhase2Threshold = 105.0f;

    // フェーズ2開始時のHP
    static constexpr float kPhase2InitialHp = 100.0f;

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
    /// 更新（deltaTime版）
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    /// <summary>
    /// スプライト描画
    /// </summary>
    void DrawSprite();

    /// <summary>
    /// ImGuiの描画
    /// </summary>
    void DrawImGui();

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damage">受けるダメージ量</param>
    /// <param name="shakeIntensityOverride">シェイク強度（0以下でデフォルト値使用）</param>
    void OnHit(float damage, float shakeIntensityOverride = 0.0f);

    /// <summary>
    /// シェイクエフェクトを開始
    /// </summary>
    /// <param name="intensity">シェイク強度（0以下でデフォルト値使用）</param>
    void StartShake(float intensity = 0.0f);

    /// <summary>
    /// 弾生成リクエストを追加
    /// </summary>
    /// <param name="position">弾の発射位置</param>
    /// <param name="velocity">弾の速度ベクトル</param>
    void RequestBulletSpawn(const Tako::Vector3& position, const Tako::Vector3& velocity);

    /// <summary>
    /// 保留中の弾生成リクエストを取得して消費
    /// </summary>
    /// <returns>弾生成リクエストのリスト（moveで返される）</returns>
    std::vector<BulletSpawnRequest> ConsumePendingBullets();

    /// <summary>
    /// 貫通弾生成リクエストを追加
    /// </summary>
    /// <param name="position">弾の発射位置</param>
    /// <param name="velocity">弾の速度ベクトル</param>
    void RequestPenetratingBulletSpawn(const Tako::Vector3& position, const Tako::Vector3& velocity);

    /// <summary>
    /// 保留中の貫通弾生成リクエストを取得して消費
    /// </summary>
    /// <returns>貫通弾生成リクエストのリスト（moveで返される）</returns>
    std::vector<BulletSpawnRequest> ConsumePendingPenetratingBullets();

    //-----------------------------Getters/Setters------------------------------//
    /// <summary>
    /// 座標変換情報を設定
    /// </summary>
    /// <param name="transform">新しい座標変換情報</param>
    void SetTransform(const Tako::Transform& transform) { transform_ = transform; }

    /// <summary>
    /// 平行移動情報を設定
    /// </summary>
    /// <param name="translate">新しい位置情報</param>
    void SetTranslate(const Tako::Vector3& translate) { transform_.translate = translate; }

    /// <summary>
    /// 回転情報を設定
    /// </summary>
    /// <param name="rotate">新しい回転情報（ラジアン）</param>
    void SetRotate(const Tako::Vector3& rotate) { transform_.rotate = rotate; }

    /// <summary>
    /// スケール情報を設定
    /// </summary>
    /// <param name="scale">新しいスケール情報</param>
    void SetScale(const Tako::Vector3& scale) { transform_.scale = scale; }

    /// <summary>
    /// HPを設定
    /// </summary>
    /// <param name="hp">新しいHP値</param>
    void SetHp(float hp) { hp_ = hp; }

    /// <summary>
    /// フェーズを設定
    /// </summary>
    /// <param name="phase">新しいフェーズ番号</param>
    void SetPhase(uint32_t phase) { phaseManager_.SetPhase(phase); }

    /// <summary>
    /// プレイヤーの参照を設定
    /// </summary>
    /// <param name="player">プレイヤーのポインタ</param>
    void SetPlayer(Player* player);

    /// <summary>
    /// 一時行動停止フラグを設定
    /// </summary>
    /// <param name="isPause">一時行動停止フラグの値</param>
    void SetIsPause(bool isPause) { isPause_ = isPause; }

    //-----------------------------スタンシステム------------------------------//
    /// <summary>
    /// スタンをトリガー（近接攻撃ヒット時に呼ばれる）
    /// スタン中は無視される（リセット防止）
    /// </summary>
    /// <param name="knockbackDirection">ノックバック方向（正規化済み）</param>
    void TriggerStun(const Tako::Vector3& knockbackDirection);

    /// <summary>
    /// スタン状態かどうか
    /// </summary>
    /// <returns>スタン中ならtrue</returns>
    bool IsStunned() const { return isStunned_; }

    /// <summary>
    /// スタンをクリア（BTBossStun終了時に呼ばれる）
    /// </summary>
    void ClearStun() { isStunned_ = false; }

    /// <summary>
    /// ノックバック方向を取得
    /// </summary>
    /// <returns>ノックバック方向ベクトル</returns>
    const Tako::Vector3& GetStunKnockbackDirection() const { return stunKnockbackDirection_; }

    /// <summary>
    /// スタン用フラッシュを開始
    /// </summary>
    /// <param name="color">フラッシュ色</param>
    /// <param name="duration">持続時間</param>
    void StartStunFlash(const Tako::Vector4& color, float duration);

    //-----------------------------硬直状態システム------------------------------//
    /// <summary>
    /// 硬直フェーズ開始（アクションノードから呼ばれる）
    /// </summary>
    void EnterRecovery();

    /// <summary>
    /// 硬直フェーズ終了（アクションノードから呼ばれる）
    /// </summary>
    void ExitRecovery();

    /// <summary>
    /// 硬直中かどうか
    /// </summary>
    /// <returns>硬直中ならtrue</returns>
    bool IsInRecovery() const { return isInRecovery_; }

    //-----------------------------ダッシュ状態システム------------------------------//
    /// <summary>
    /// ダッシュ状態を設定
    /// </summary>
    /// <param name="dashing">ダッシュ中ならtrue</param>
    void SetDashing(bool dashing);

    /// <summary>
    /// ダッシュ中かどうか
    /// </summary>
    /// <returns>ダッシュ中ならtrue</returns>
    bool IsDashing() const { return isDashing_; }

    //-----------------------------離脱トリガーシステム------------------------------//
    /// <summary>
    /// 離脱をトリガー（硬直時間外に近接攻撃を受けた時に呼ばれる）
    /// </summary>
    /// <param name="direction">離脱方向（正規化済み）</param>
    void TriggerRetreat(const Tako::Vector3& direction);

    /// <summary>
    /// 離脱すべきかどうか
    /// </summary>
    /// <returns>離脱要求があればtrue</returns>
    bool ShouldRetreat() const { return shouldRetreat_; }

    /// <summary>
    /// 離脱フラグをクリア（BTBossRetreat終了時に呼ばれる）
    /// </summary>
    void ClearRetreat() { shouldRetreat_ = false; }

    /// <summary>
    /// 離脱方向を取得
    /// </summary>
    /// <returns>離脱方向ベクトル</returns>
    const Tako::Vector3& GetRetreatDirection() const { return retreatDirection_; }

    /// <summary>
    /// 座標変換情報を取得
    /// </summary>
    /// <returns>現在の座標変換情報の参照</returns>
    const Tako::Transform& GetTransform() const { return transform_; }

    /// <summary>
    /// 座標変換情報を取得（非const版）
    /// </summary>
    /// <returns>現在の座標変換情報の参照</returns>
    Tako::Transform& GetWorldTransform() { return transform_; }

    /// <summary>
    /// 座標変換情報のポインタを取得
    /// </summary>
    /// <returns>座標変換情報への非constポインタ</returns>
    Tako::Transform* GetTransformPtr() { return &transform_; }

    /// <summary>
    /// 平行移動情報を取得
    /// </summary>
    /// <returns>現在の位置情報の参照</returns>
    Tako::Vector3 GetTranslate() const { return transform_.translate; }

    /// <summary>
    /// 回転情報を取得
    /// </summary>
    /// <returns>現在の回転情報の参照（ラジアン）</returns>
    Tako::Vector3 GetRotate() const { return transform_.rotate; }

    /// <summary>
    /// スケール情報を取得
    /// </summary>
    /// <returns>現在のスケール情報の参照</returns>
    Tako::Vector3 GetScale() const { return transform_.scale; }

    /// <summary>
    /// プレイヤーを取得
    /// </summary>
    /// <returns>プレイヤーのポインタ</returns>
    Player* GetPlayer() const { return player_; }

    /// <summary>
    /// HPを取得
    /// </summary>
    /// <returns>現在のHP値</returns>
    float GetHp() const { return hp_; }

    /// <summary>
    /// 最大HPを取得
    /// </summary>
    /// <returns>最大HP値</returns>
    static constexpr float GetMaxHp() { return kMaxHp; }

    /// <summary>
    /// 現在のフェーズを取得
    /// </summary>
    /// <returns>フェーズ番号</returns>
    uint32_t GetPhase() const { return phaseManager_.GetPhase(); }

    /// <summary>
    /// 死亡状態を取得
    /// </summary>
    /// <returns> 死亡しているかどうかの真偽値 </returns>
    bool IsDead() const { return phaseManager_.IsDead(); }

    /// <summary>
    /// フェーズマネージャーを取得
    /// </summary>
    /// <returns>フェーズマネージャーの参照</returns>
    BossPhaseManager& GetPhaseManager() { return phaseManager_; }

    /// <summary>
    /// コライダーを取得
    /// </summary>
    /// <returns>ボスのOBBコライダーのポインタ</returns>
    Tako::OBBCollider* GetCollider() const { return bodyCollider_.get(); }

    //-----------------------------近接攻撃関連------------------------------//
    /// <summary>
    /// 攻撃ブロックを取得
    /// </summary>
    /// <returns>攻撃ブロックのObject3dポインタ</returns>
    Tako::Object3d* GetMeleeAttackBlock() const { return meleeAttackBlock_.get(); }

    /// <summary>
    /// 攻撃ブロックの表示/非表示を設定
    /// </summary>
    /// <param name="visible">表示する場合true</param>
    void SetMeleeAttackBlockVisible(bool visible) { meleeAttackBlockVisible_ = visible; }

    /// <summary>
    /// 攻撃ブロックが表示中か取得
    /// </summary>
    /// <returns>表示中の場合true</returns>
    bool IsMeleeAttackBlockVisible() const { return meleeAttackBlockVisible_; }

    /// <summary>
    /// 近接攻撃コライダーを取得
    /// </summary>
    /// <returns>近接攻撃コライダーのポインタ</returns>
    BossMeleeAttackCollider* GetMeleeAttackCollider() const { return meleeAttackCollider_.get(); }

    /// <summary>
    /// 予兆エフェクトをアクティブ化/非アクティブ化
    /// </summary>
    /// <param name="active">アクティブにする場合true</param>
    void SetAttackSignEmitterActive(bool active);

    /// <summary>
    /// 予兆エフェクトの位置を設定
    /// </summary>
    /// <param name="position">設定する位置</param>
    void SetAttackSignEmitterPosition(const Tako::Vector3& position);

    //-----------------------------射撃予兆エフェクト------------------------------//
    /// <summary>
    /// 射撃予兆エフェクトをアクティブ化/非アクティブ化
    /// </summary>
    /// <param name="active">アクティブにする場合true</param>
    void SetBulletSignEmitterActive(bool active);

    /// <summary>
    /// 射撃予兆エフェクトの位置を設定
    /// </summary>
    /// <param name="position">設定する位置</param>
    void SetBulletSignEmitterPosition(const Tako::Vector3& position);

    /// <summary>
    /// 射撃予兆エフェクトのスケール範囲Xを設定
    /// </summary>
    /// <param name="value">スケール値（min=max=value）</param>
    void SetBulletSignEmitterScaleRangeX(float value);

    /// <summary>
    /// EmitterManagerを設定
    /// </summary>
    /// <param name="emitterManager">EmitterManagerのポインタ</param>
    void SetEmitterManager(Tako::EmitterManager* emitterManager) { emitterManager_ = emitterManager; }

private:
    // ボスの3Dモデルオブジェクト（描画とアニメーション管理）
    std::unique_ptr<Tako::Object3d> model_;

    // ボスの座標変換情報（位置、回転、スケール）
    Tako::Transform transform_{};

    // ビヘイビアツリー
    std::unique_ptr<BossBehaviorTree> behaviorTree_;

#ifdef _DEBUG
    // ビヘイビアツリーノードエディタ
    std::unique_ptr<BossNodeEditor> nodeEditor_;

    // ノードエディタの表示フラグ
    bool showNodeEditor_ = false;
#endif

    // プレイヤーへの参照
    Player* player_ = nullptr;

    // ボスの現在HP（0になると撃破、初期値200）
    float hp_ = kMaxHp;

    // フェーズ・ライフ管理
    BossPhaseManager phaseManager_;

    // 一時行動停止フラグ
    bool isPause_ = false;

    // ===== スタンシステム =====
    bool isStunned_ = false;                      ///< スタン状態フラグ
    Tako::Vector3 stunKnockbackDirection_;        ///< ノックバック方向

    // ===== 硬直状態システム =====
    bool isInRecovery_ = false;                   ///< 硬直中フラグ
    bool isDashing_ = false;                      ///< ダッシュ中フラグ

    // ===== 離脱トリガーシステム =====
    bool shouldRetreat_ = false;                  ///< 離脱要求フラグ
    Tako::Vector3 retreatDirection_;              ///< 離脱方向

    // ボス本体の衝突判定用AABBコライダー
    std::unique_ptr<Tako::OBBCollider> bodyCollider_;

    //-----------------------------近接攻撃関連------------------------------//
    // 近接攻撃用武器ブロック
    std::unique_ptr<Tako::Object3d> meleeAttackBlock_;

    // 攻撃ブロック表示フラグ
    bool meleeAttackBlockVisible_ = false;

    // 近接攻撃用コライダー
    std::unique_ptr<BossMeleeAttackCollider> meleeAttackCollider_;

    // 予兆エフェクト管理
    Tako::EmitterManager* emitterManager_ = nullptr;

    // 予兆エフェクト名
    std::string attackSignEmitterName_ = "boss_melee_attack_sign";
    std::string bulletSignEmitterName_ = "boss_bullet_sign";

    // ===== エフェクト関連 =====
    HitFlashEffect hitFlashEffect_;     ///< ヒット時の色変化エフェクト
    ShakeEffect shakeEffect_;           ///< シェイクエフェクト

    // 弾生成管理
    BulletSpawner bulletSpawner_;              ///< 通常弾生成リクエスト管理
    BulletSpawner penetratingBulletSpawner_;   ///< 貫通弾生成リクエスト管理

    // HPバーUI
    HPBarUI hpBar_;                      ///< HPバー表示

    // 初期座標
    float initialY_ = 2.5f;   ///< 初期Y座標
    float initialZ_ = 10.0f;  ///< 初期Z座標
};

