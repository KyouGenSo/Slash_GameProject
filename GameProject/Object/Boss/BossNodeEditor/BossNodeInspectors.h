#pragma once
#ifdef _DEBUG

#include <json.hpp>
#include <memory>
#include "../../../BehaviorTree/Core/BTNode.h"

class BTBossDash;
class BTBossShoot;
class BTBossRapidFire;
class BTBossIdle;
class BTActionSelector;

/// <summary>
/// ノードインスペクター基底インターフェース
/// 各ノードタイプ固有のUI描画とパラメータ管理を担当
/// </summary>
class IBTNodeInspector {
public:
    virtual ~IBTNodeInspector() = default;

    /// <summary>
    /// インスペクターUIを描画
    /// </summary>
    /// <returns>パラメータ変更があればtrue</returns>
    virtual bool DrawUI() = 0;

    /// <summary>
    /// パラメータをJSONに抽出（保存用）
    /// </summary>
    virtual nlohmann::json ExtractParams() const = 0;

    /// <summary>
    /// JSONからパラメータを適用（読み込み用）
    /// </summary>
    virtual void ApplyParams(const nlohmann::json& params) = 0;
};

// ========== BTBossDash用インスペクター ==========
class BTBossDashInspector : public IBTNodeInspector {
public:
    explicit BTBossDashInspector(BTBossDash* node) : node_(node) {}
    bool DrawUI() override;
    nlohmann::json ExtractParams() const override;
    void ApplyParams(const nlohmann::json& params) override;
private:
    BTBossDash* node_;
};

// ========== BTBossShoot用インスペクター ==========
class BTBossShootInspector : public IBTNodeInspector {
public:
    explicit BTBossShootInspector(BTBossShoot* node) : node_(node) {}
    bool DrawUI() override;
    nlohmann::json ExtractParams() const override;
    void ApplyParams(const nlohmann::json& params) override;
private:
    BTBossShoot* node_;
};

// ========== BTBossRapidFire用インスペクター ==========
class BTBossRapidFireInspector : public IBTNodeInspector {
public:
    explicit BTBossRapidFireInspector(BTBossRapidFire* node) : node_(node) {}
    bool DrawUI() override;
    nlohmann::json ExtractParams() const override;
    void ApplyParams(const nlohmann::json& params) override;
private:
    BTBossRapidFire* node_;
};

// ========== BTBossIdle用インスペクター ==========
class BTBossIdleInspector : public IBTNodeInspector {
public:
    explicit BTBossIdleInspector(BTBossIdle* node) : node_(node) {}
    bool DrawUI() override;
    nlohmann::json ExtractParams() const override;
    void ApplyParams(const nlohmann::json& params) override;
private:
    BTBossIdle* node_;
};

// ========== BTActionSelector用インスペクター ==========
class BTActionSelectorInspector : public IBTNodeInspector {
public:
    explicit BTActionSelectorInspector(BTActionSelector* node) : node_(node) {}
    bool DrawUI() override;
    nlohmann::json ExtractParams() const override;
    void ApplyParams(const nlohmann::json& params) override;
private:
    BTActionSelector* node_;
};

/// <summary>
/// ノードに対応するインスペクターを生成するファクトリ関数
/// </summary>
/// <param name="node">対象のランタイムノード</param>
/// <returns>対応するインスペクター（対応なしならnullptr）</returns>
std::unique_ptr<IBTNodeInspector> CreateNodeInspector(const BTNodePtr& node);

#endif // _DEBUG
