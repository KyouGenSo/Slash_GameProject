#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"

/// <summary>
/// フェーズ条件ノード
/// 現在のボスフェーズを指定した値と比較して成功/失敗を返す
/// </summary>
class BTBossPhaseCondition : public BTNode {
public:
    /// <summary>
    /// 比較タイプ
    /// </summary>
    enum class Comparison {
        Equal = 0,           // フェーズが一致
        NotEqual = 1,        // フェーズが不一致
        GreaterOrEqual = 2,  // フェーズが以上
        LessOrEqual = 3      // フェーズが以下
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTBossPhaseCondition();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTBossPhaseCondition() = default;

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

    // Getters/Setters
    uint32_t GetTargetPhase() const { return targetPhase_; }
    void SetTargetPhase(uint32_t phase) { targetPhase_ = phase; }

    Comparison GetComparison() const { return comparison_; }
    void SetComparison(Comparison comp) { comparison_ = comp; }

private:
    /// <summary>
    /// 比較条件を評価
    /// </summary>
    /// <param name="currentPhase">現在のフェーズ</param>
    /// <returns>条件を満たしていればtrue</returns>
    bool EvaluateCondition(uint32_t currentPhase) const;

    // 目標フェーズ
    uint32_t targetPhase_ = 2;

    // 比較タイプ
    Comparison comparison_ = Comparison::Equal;
};
