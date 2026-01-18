#pragma once
#include "Vector3.h"

class Boss;

/// <summary>
/// ボス射撃予備動作エフェクト管理クラス
/// チャージ期間中にエミッターを表示し、スケールを補間する
/// </summary>
class BulletSignEffect {
public:
    /// <summary>
    /// エフェクト開始
    /// </summary>
    /// <param name="boss">ボス</param>
    /// <param name="duration">持続時間（秒）</param>
    void Start(Boss* boss, float duration);

    /// <summary>
    /// 毎フレーム更新
    /// </summary>
    /// <param name="boss">ボス</param>
    /// <param name="deltaTime">経過時間</param>
    void Update(Boss* boss, float deltaTime);

    /// <summary>
    /// エフェクト終了
    /// </summary>
    /// <param name="boss">ボス</param>
    void End(Boss* boss);

    /// <summary>
    /// エフェクトがアクティブか
    /// </summary>
    bool IsActive() const { return isActive_; }

private:
    /// <summary>
    /// ボス前方のエミッター位置を計算
    /// </summary>
    /// <param name="boss">ボス</param>
    /// <returns>エミッター位置</returns>
    Tako::Vector3 CalculateEmitterPosition(Boss* boss);

    static constexpr float kForwardDistance = 2.0f;   ///< ボス前方距離
    static constexpr float kScaleMin = 0.01f;         ///< スケール最小値
    static constexpr float kScaleMax = 15.0f;         ///< スケール最大値

    float duration_ = 0.9f;      ///< 持続時間
    float elapsedTime_ = 0.0f;   ///< 経過時間
    bool isActive_ = false;      ///< アクティブフラグ
};
