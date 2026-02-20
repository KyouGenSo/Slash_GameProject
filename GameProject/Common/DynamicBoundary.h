#pragma once
#include "Vector3.h"
#include <algorithm>

/// <summary>
/// 動的移動範囲管理構造体
/// オブジェクトの移動可能範囲を制限するために使用
/// ヘッダーオンリー実装
/// </summary>
struct DynamicBoundary
{
    /// <summary>
    /// 境界無効化マーカー値
    /// </summary>
    static constexpr float kDisabledMarker = 9999.0f;

    float xMin = -kDisabledMarker;  ///< X 座標の最小値
    float xMax = kDisabledMarker;   ///< X 座標の最大値
    float zMin = -kDisabledMarker;  ///< Z 座標の最小値
    float zMax = kDisabledMarker;   ///< Z 座標の最大値

    /// <summary>
    /// 境界を直接設定
    /// </summary>
    /// <param name="minX">X 座標の最小値</param>
    /// <param name="maxX">X 座標の最大値</param>
    /// <param name="minZ">Z 座標の最小値</param>
    /// <param name="maxZ">Z 座標の最大値</param>
    void Set(float minX, float maxX, float minZ, float maxZ)
    {
        xMin = minX;
        xMax = maxX;
        zMin = minZ;
        zMax = maxZ;
    }

    /// <summary>
    /// 中心点と範囲から境界を設定
    /// </summary>
    /// <param name="center">中心座標</param>
    /// <param name="xRange">X 方向の範囲（片側）</param>
    /// <param name="zRange">Z 方向の範囲（片側）</param>
    void SetFromCenter(const Tako::Vector3& center, float xRange, float zRange)
    {
        xMin = center.x - xRange;
        xMax = center.x + xRange;
        zMin = center.z - zRange;
        zMax = center.z + zRange;
    }

    /// <summary>
    /// 境界をクリア（無効化）
    /// </summary>
    void Clear()
    {
        xMin = -kDisabledMarker;
        xMax = kDisabledMarker;
        zMin = -kDisabledMarker;
        zMax = kDisabledMarker;
    }

    /// <summary>
    /// 境界が有効か（設定されているか）
    /// </summary>
    /// <returns>有効なら true</returns>
    bool IsEnabled() const
    {
        // いずれかの値がマーカー値でなければ有効
        return xMin > -kDisabledMarker || xMax < kDisabledMarker ||
            zMin > -kDisabledMarker || zMax < kDisabledMarker;
    }

    /// <summary>
    /// 位置を境界内にクランプ
    /// </summary>
    /// <param name="position">クランプする位置</param>
    /// <returns>クランプ後の位置</returns>
    Tako::Vector3 Clamp(const Tako::Vector3& position) const
    {
        Tako::Vector3 result = position;
        result.x = std::clamp(result.x, xMin, xMax);
        result.z = std::clamp(result.z, zMin, zMax);
        return result;
    }

    /// <summary>
    /// 位置が境界内にあるか
    /// </summary>
    /// <param name="position">判定する位置</param>
    /// <returns>境界内なら true</returns>
    bool Contains(const Tako::Vector3& position) const
    {
        return position.x >= xMin && position.x <= xMax &&
            position.z >= zMin && position.z <= zMax;
    }
};
