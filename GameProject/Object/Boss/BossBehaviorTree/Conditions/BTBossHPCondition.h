#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"

/// <summary>
/// HP条件ノード
/// 現在のボスHPを最大HPに対するパーセンテージで比較して成功/失敗を返す
/// </summary>
class BTBossHPCondition : public BTNode {
public:
    /// <summary>
    /// 比較タイプ
    /// </summary>
    enum class Comparison {
        Less = 0,            // HPが閾値未満
        LessOrEqual = 1,     // HPが閾値以下
        Greater = 2,         // HPが閾値超過
        GreaterOrEqual = 3   // HPが閾値以上
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTBossHPCondition();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTBossHPCondition() = default;

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
    float GetThresholdPercent() const { return thresholdPercent_; }
    void SetThresholdPercent(float percent) { thresholdPercent_ = percent; }

    Comparison GetComparison() const { return comparison_; }
    void SetComparison(Comparison comp) { comparison_ = comp; }

private:
    /// <summary>
    /// 比較条件を評価
    /// </summary>
    /// <param name="currentPercent">現在のHPパーセンテージ</param>
    /// <returns>条件を満たしていればtrue</returns>
    bool EvaluateCondition(float currentPercent) const;

    // HP閾値（パーセンテージ: 0〜100）
    float thresholdPercent_ = 50.0f;

    // 比較タイプ
    Comparison comparison_ = Comparison::LessOrEqual;
};
