#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"

/// <summary>
/// スタン状態チェック条件ノード
/// ボスがスタン状態ならSuccess、そうでなければFailure
/// </summary>
class BTCheckStunned : public BTNode {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTCheckStunned();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTCheckStunned() = default;

    /// <summary>
    /// ノードの実行
    /// </summary>
    /// <param name="blackboard">ブラックボード</param>
    /// <returns>スタン中ならSuccess、そうでなければFailure</returns>
    BTNodeStatus Execute(BTBlackboard* blackboard) override;

    /// <summary>
    /// ノードのリセット
    /// </summary>
    void Reset() override;

    /// <summary>
    /// JSONからパラメータを適用（パラメータなし）
    /// </summary>
    /// <param name="params">パラメータJSON</param>
    void ApplyParameters(const nlohmann::json& params) override { (void)params; }

    /// <summary>
    /// パラメータをJSONとして抽出（パラメータなし）
    /// </summary>
    nlohmann::json ExtractParameters() const override { return {}; }

#ifdef _DEBUG
    /// <summary>
    /// ImGuiでパラメータ編集UIを描画
    /// </summary>
    bool DrawImGui() override;
#endif
};
