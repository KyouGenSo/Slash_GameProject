#pragma once
#include "Camera.h"
#include "Transform.h"
#include <memory>
#include <vector>

/// <summary>
/// カメラコントローラーの優先度定義
/// 数値が大きいほど優先度が高い
/// </summary>
enum class CameraControlPriority {
    USER_INPUT = 0,        ///< ユーザー入力（最低優先度）
    FOLLOW_DEFAULT = 50,   ///< デフォルト追従
    SCRIPTED_EVENT = 75,   ///< スクリプトイベント
    ANIMATION = 100,       ///< アニメーション再生（最高優先度）
    DEBUG_OVERRIDE = 999   ///< デバッグ用強制オーバーライド
};

/// <summary>
/// カメラコントローラーインターフェース
/// 単一責任原則に基づく設計
/// </summary>
class ICameraController {
public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~ICameraController() = default;

    /// <summary>
    /// 更新処理
    /// </summary>
    /// <param name="deltaTime">前フレームからの経過時間（秒）</param>
    virtual void Update(float deltaTime) = 0;

    /// <summary>
    /// このコントローラーがアクティブかを判定
    /// </summary>
    /// <returns>アクティブな場合 true</returns>
    virtual bool IsActive() const = 0;

    /// <summary>
    /// 優先度を取得
    /// </summary>
    /// <returns>コントローラーの優先度</returns>
    virtual CameraControlPriority GetPriority() const = 0;

    /// <summary>
    /// コントローラーをアクティブ化
    /// </summary>
    virtual void Activate() = 0;

    /// <summary>
    /// コントローラーを非アクティブ化
    /// </summary>
    virtual void Deactivate() = 0;

    /// <summary>
    /// カメラを設定
    /// </summary>
    /// <param name="camera">制御対象のカメラ</param>
    virtual void SetCamera(Tako::Camera* camera) { camera_ = camera; }

    /// <summary>
    /// カメラを取得
    /// </summary>
    /// <returns>制御対象のカメラ</returns>
    Tako::Camera* GetCamera() const { return camera_; }

protected:
    Tako::Camera* camera_ = nullptr;  ///< 制御対象のカメラ
    bool isActive_ = false;     ///< アクティブ状態
};

/// <summary>
/// ターゲット追従型コントローラーの基底クラス
/// </summary>
class TargetedCameraController : public ICameraController {
public:
    /// <summary>
    /// ターゲットを設定
    /// </summary>
    /// <param name="target">追従対象の Transform</param>
    virtual void SetTarget(const Tako::Transform* target) {
        primaryTarget_ = target;
    }

    /// <summary>
    /// 追加ターゲットを設定（マルチターゲット対応）
    /// </summary>
    /// <param name="targets">追従対象の Transform 配列</param>
    virtual void SetAdditionalTargets(const std::vector<const Tako::Transform*>& targets) {
        additionalTargets_ = targets;
    }

protected:
    const Tako::Transform* primaryTarget_ = nullptr;              ///< 主要ターゲット
    std::vector<const Tako::Transform*> additionalTargets_;       ///< 追加ターゲット配列
};