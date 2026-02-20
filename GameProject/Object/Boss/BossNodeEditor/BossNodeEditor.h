#pragma once

#ifdef _DEBUG

#include "ImGuiManager.h"
#include <../../../../TakoEngine/project/externals/imgui-node-editor/imgui_node_editor.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <string>
#include <json.hpp>
#include "../../../BehaviorTree/Core/BTNode.h"

// 名前空間エイリアス
namespace ed = ax::NodeEditor;

class BossBehaviorTree;
class Boss;
class Player;

/// <summary>
/// ボス用ビヘイビアツリーノードエディタ
/// imgui-node-editor を直接使用してビヘイビアツリーを視覚的に編集
/// </summary>
class BossNodeEditor {
public:
    /// <summary>
    /// エディタノードデータ（エディタ専用メタデータ）
    /// </summary>
    struct EditorNode {
        int id;                              // エディタ固有 ID（10000番台）
        ImVec2 position;                     // エディタ上の位置
        std::string nodeType;                // ノードタイプ名（"BTSelector", "BTSequence"等）
        std::string displayName;             // 表示名
        BTNodePtr runtimeNode;               // 実際の実行ノード
        std::vector<int> inputPinIds;        // 入力ピン ID（親接続用）
        std::vector<int> outputPinIds;       // 出力ピン ID（子接続用）
        ImVec4 color;                        // ノードカラー
    };

    /// <summary>
    /// エディタリンクデータ
    /// </summary>
    struct EditorLink {
        int id;                              // リンク固有 ID（30000番台）
        int startPinId;                      // 開始ピン ID
        int endPinId;                        // 終了ピン ID
        int startNodeId;                     // 開始ノード ID
        int endNodeId;                       // 終了ノード ID
    };

    /// <summary>
    /// エディタピンデータ
    /// </summary>
    struct EditorPin {
        int id;                              // ピン固有 ID（20000番台）
        int nodeId;                          // 所属ノード ID
        bool isInput;                        // 入力ピンかどうか
        std::string name;                    // ピン名
    };

    /// <summary>
    /// コンストラクタ
    /// </summary>
    BossNodeEditor();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~BossNodeEditor();

    /// <summary>
    /// エディタの初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// エディタの更新・描画（ImGui ウィンドウ内で呼ぶ）
    /// </summary>
    void Update();

    /// <summary>
    /// エディタの終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// エディタの表示切り替え
    /// </summary>
    /// <param name="visible">表示するかどうか</param>
    void SetVisible(bool visible) { isVisible_ = visible; }

    /// <summary>
    /// エディタの表示状態取得
    /// </summary>
    /// <returns>表示中なら true</returns>
    bool IsVisible() const { return isVisible_; }

    /// <summary>
    /// ツリーを JSON から読み込み
    /// </summary>
    /// <param name="filepath">JSON ファイルパス</param>
    /// <returns>成功したら true</returns>
    bool LoadFromJSON(const std::string& filepath);

    /// <summary>
    /// ツリーを JSON に保存
    /// </summary>
    /// <param name="filepath">JSON ファイルパス</param>
    /// <returns>成功したら true</returns>
    bool SaveToJSON(const std::string& filepath);

    /// <summary>
    /// 実行時ツリーを構築（BossBehaviorTree に渡す用）
    /// </summary>
    /// <returns>ルートノード</returns>
    BTNodePtr BuildRuntimeTree();

    /// <summary>
    /// 現在実行中のノードをハイライト表示（デバッグ用）
    /// </summary>
    /// <param name="nodePtr">実行中のノード</param>
    void HighlightRunningNode(const BTNodePtr& nodePtr);

    /// <summary>
    /// エディタのクリア
    /// </summary>
    void Clear();
private:
    void DrawNodes();
    void DrawNode(const EditorNode& node);
    void DrawLinks();
    void DrawPin(const EditorPin& pin);
    void HandleLinkCreation();
    void HandleDeletion();
    void DrawContextMenu();
    void DrawNodeInspector();
    void DrawToolbar();

    // ノード作成
    void CreateNode(const std::string& nodeType, const ImVec2& position);
    int CreateNodeWithId(int nodeId, const std::string& nodeType, const ImVec2& position);

    // リンク作成ヘルパー
    bool CreateLink(int sourceNodeId, int targetNodeId);

    // ヘルパー関数
    EditorNode* FindNodeById(int nodeId);
    const EditorNode* FindNodeById(int nodeId) const;
    EditorNode* FindNodeByRuntimeNode(const BTNodePtr& node);
    EditorPin* FindPinById(int pinId);
    const EditorPin* FindPinById(int pinId) const;
    EditorLink* FindLinkById(int linkId);

    int FindRootNodeId() const;
    void BuildRuntimeTreeRecursive(int nodeId, BTNodePtr& outNode);
    bool HasCyclicDependency(int startNodeId, int endNodeId) const;
    std::vector<int> GetChildNodeIds(int parentNodeId) const;

    // ノードパラメータの保存・復元
    nlohmann::json ExtractNodeParameters(const EditorNode& node);
    void ApplyNodeParameters(EditorNode& node, const nlohmann::json& params);

private:
    // ax::NodeEditor コンテキスト
    ed::EditorContext* editorContext_;
    std::unique_ptr<ed::Config> editorConfig_;

    // エディタデータ
    std::vector<EditorNode> nodes_;
    std::vector<EditorLink> links_;
    std::vector<EditorPin> pins_;

    // ID 管理（ID 範囲を分離して競合を防ぐ）
    int nextNodeId_;    // 10000番台
    int nextLinkId_;    // 30000番台
    int nextPinId_;     // 20000番台

    // エディタ状態
    bool isVisible_;
    bool firstFrame_;
    int highlightedNodeId_;  // 現在ハイライト中のノード ID（実行デバッグ用）
    float highlightStartTime_;  // ハイライト開始時刻（パルスエフェクト用）
    int selectedNodeId_ = -1;  // 選択中のノード ID（インスペクター用）

    // ノード・ピン ID マッピング管理
    std::unordered_map<BTNode*, int> runtimeNodeToEditorId_;
};

#endif // _DEBUG