#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"
#include "Vector3.h"
#include "Vector4.h"

class Boss;

/// <summary>
/// ボスのスタンアクションノード
/// ノックバック移動と色点滅を実行し、一定時間後に終了
/// </summary>
class BTBossStun : public BTNode {
    //=========================================================================================
    // 定数
    //=========================================================================================
private:
    static constexpr float kEasingCoeffA = 3.0f;       ///< イージング係数A
    static constexpr float kEasingCoeffB = 2.0f;       ///< イージング係数B
    static constexpr float kDirectionEpsilon = 0.01f;  ///< 方向判定の閾値

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTBossStun();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTBossStun() = default;

    /// <summary>
    /// ノードの実行
    /// </summary>
    /// <param name="blackboard">ブラックボード</param>
    /// <returns>実行結果</returns>
    BTNodeStatus Execute(BTBlackboard* blackboard) override;

    /// <summary>
    /// ノードのリセット
    /// </summary>
    void Reset() override;

    // パラメータ取得・設定
    float GetStunDuration() const { return stunDuration_; }
    void SetStunDuration(float duration) { stunDuration_ = duration; }
    float GetKnockbackDistance() const { return knockbackDistance_; }
    void SetKnockbackDistance(float distance) { knockbackDistance_ = distance; }
    float GetKnockbackDuration() const { return knockbackDuration_; }
    void SetKnockbackDuration(float duration) { knockbackDuration_ = duration; }
    float GetFlashInterval() const { return flashInterval_; }
    void SetFlashInterval(float interval) { flashInterval_ = interval; }

    /// <summary>
    /// JSONからパラメータを適用
    /// </summary>
    /// <param name="params">パラメータJSON</param>
    void ApplyParameters(const nlohmann::json& params) override;

    /// <summary>
    /// パラメータをJSONとして抽出
    /// </summary>
    nlohmann::json ExtractParameters() const override;

#ifdef _DEBUG
    /// <summary>
    /// ImGuiでパラメータ編集UIを描画
    /// </summary>
    bool DrawImGui() override;
#endif

private:
    /// <summary>
    /// スタン開始時の初期化
    /// </summary>
    /// <param name="boss">ボス</param>
    void InitializeStun(Boss* boss);

    /// <summary>
    /// ノックバック移動の更新
    /// </summary>
    /// <param name="boss">ボス</param>
    void UpdateKnockback(Boss* boss);

    /// <summary>
    /// 色点滅の更新
    /// </summary>
    /// <param name="boss">ボス</param>
    void UpdateFlash(Boss* boss);

    /// <summary>
    /// エリア内に収まる位置を計算
    /// </summary>
    /// <param name="position">調整前の位置</param>
    /// <returns>エリア内に収まる位置</returns>
    Tako::Vector3 ClampToArea(const Tako::Vector3& position);

    //=========================================================================================
    // パラメータ
    //=========================================================================================
    float stunDuration_ = 1.5f;           ///< スタン時間（秒）
    float knockbackDistance_ = 8.0f;      ///< ノックバック距離
    float knockbackDuration_ = 0.3f;      ///< ノックバック移動時間
    float flashInterval_ = 0.05f;         ///< 色点滅間隔（速いペース）
    float flashDuration_ = 0.03f;         ///< 各フラッシュの持続時間
    Tako::Vector4 stunFlashColor_ = {1.0f, 1.0f, 0.0f, 1.0f};  ///< 黄色

    //=========================================================================================
    // 状態管理
    //=========================================================================================
    Tako::Vector3 startPosition_;         ///< 開始位置
    Tako::Vector3 targetPosition_;        ///< 目標位置
    float elapsedTime_ = 0.0f;            ///< 経過時間
    float flashTimer_ = 0.0f;             ///< フラッシュタイマー
    bool isFirstExecute_ = true;          ///< 初回実行フラグ
    bool knockbackComplete_ = false;      ///< ノックバック完了フラグ
    float knockbackTimer_ = 0.0f;         ///< ノックバック専用タイマー
    bool knockbackWasSkipped_ = false;    ///< ノックバックがスキップされたフラグ
};
