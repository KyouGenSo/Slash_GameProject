#pragma once
#include "../../../../BehaviorTree/Core/BTNode.h"
#include "../../../../BehaviorTree/Core/BTBlackboard.h"

/// <summary>
/// 離脱要求チェック条件ノード
/// ボスが離脱すべき状態ならSuccess、そうでなければFailure
/// </summary>
class BTCheckRetreat : public BTNode {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    BTCheckRetreat();

    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~BTCheckRetreat() = default;

    /// <summary>
    /// ノードの実行
    /// </summary>
    /// <param name="blackboard">ブラックボード</param>
    /// <returns>離脱要求があればSuccess、そうでなければFailure</returns>
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
