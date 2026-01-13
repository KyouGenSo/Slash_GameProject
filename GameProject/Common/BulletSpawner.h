#pragma once
#include <vector>
#include "Vector3.h"
#include "BulletSpawnRequest.h"

/// <summary>
/// 弾生成リクエスト管理クラス
/// Player/Boss等のオブジェクトが弾の生成をGameSceneに要求する際に使用
/// コンポジション方式で使用する（継承不要）
/// </summary>
class BulletSpawner
{
public:
    BulletSpawner() = default;
    ~BulletSpawner() = default;

    /// <summary>
    /// 弾生成リクエストを追加
    /// </summary>
    /// <param name="position">弾の発射位置</param>
    /// <param name="velocity">弾の速度ベクトル</param>
    void RequestSpawn(const Tako::Vector3& position, const Tako::Vector3& velocity);

    /// <summary>
    /// 保留中の弾生成リクエストを取得して消費
    /// </summary>
    /// <returns>弾生成リクエストのリスト（moveで返される）</returns>
    std::vector<BulletSpawnRequest> Consume();

    /// <summary>
    /// 保留中のリクエストがあるか
    /// </summary>
    /// <returns>保留中のリクエストがある場合true</returns>
    bool HasPending() const { return !pendingBullets_.empty(); }

    /// <summary>
    /// 保留中のリクエスト数を取得
    /// </summary>
    /// <returns>保留中のリクエスト数</returns>
    size_t GetPendingCount() const { return pendingBullets_.size(); }

    /// <summary>
    /// 保留中のリクエストをクリア
    /// </summary>
    void Clear() { pendingBullets_.clear(); }

private:
    std::vector<BulletSpawnRequest> pendingBullets_;
};
