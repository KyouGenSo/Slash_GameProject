#pragma once
#include "Camera.h"
#include "Transform.h"
#include "CameraSystem/CameraManager.h"
#include <memory>
#include <string>

class CameraAnimation;
class FirstPersonController;
class TopDownController;

/// <summary>
/// リファクタリング版FollowCameraクラス
/// 新アーキテクチャのファサードとして機能
/// 既存のインターフェースを維持しつつ内部実装を改善
/// </summary>
class FollowCamera {
public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    FollowCamera();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~FollowCamera();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="camera">制御対象のカメラ</param>
    void Initialize(Camera* camera);

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize();

    /// <summary>
    /// 更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// 一人称視点モードに切り替え
    /// </summary>
    void FirstPersonMode();

    /// <summary>
    /// トップダウン視点モードに切り替え
    /// </summary>
    void TopDownMode();

    /// <summary>
    /// カメラ位置をリセット
    /// </summary>
    void Reset();

    /// <summary>
    /// ゲーム開始時のカメラアニメーション再生
    /// </summary>
    void PlayStartCameraAnimation();

    /// <summary>
    /// ImGuiデバッグ情報の描画
    /// </summary>
    void DrawImGui();

    //==================== Setter ====================

    /// <summary>
    /// 追従ターゲットを設定
    /// </summary>
    /// <param name="target">追従対象のTransform</param>
    void SetTarget(const Transform* target);

    /// <summary>
    /// 第二ターゲットを設定（マルチターゲット対応）
    /// </summary>
    /// <param name="target2">第二ターゲットのTransform</param>
    void SetTarget2(const Transform* target2);

    /// <summary>
    /// カメラオフセットを設定
    /// </summary>
    /// <param name="offset">新しいオフセット値</param>
    void SetOffset(const Vector3& offset);

    /// <summary>
    /// 回転速度を設定
    /// </summary>
    /// <param name="speed">新しい回転速度</param>
    void SetRotateSpeed(float speed);

    /// <summary>
    /// カメラモードを設定
    /// </summary>
    /// <param name="mode">true: 一人称視点, false: トップダウン視点</param>
    void SetMode(bool mode);

    //==================== Getter ====================

    /// <summary>
    /// カメラを取得
    /// </summary>
    /// <returns>現在のカメラのポインタ</returns>
    Camera* GetCamera();

    /// <summary>
    /// ビュープロジェクション行列を取得
    /// </summary>
    /// <returns>ビュープロジェクション行列の参照</returns>
    const Matrix4x4& GetViewProjection() const;

    /// <summary>
    /// オフセット値を取得
    /// </summary>
    /// <returns>現在のオフセットベクトル</returns>
    Vector3 GetOffset() const;

    /// <summary>
    /// 第二ターゲットを取得
    /// </summary>
    /// <returns>第二ターゲットのTransformポインタ</returns>
    const Transform* GetTarget2() const;

    /// <summary>
    /// カメラモードを取得
    /// </summary>
    /// <returns>true: 一人称視点, false: トップダウン視点</returns>
    bool GetMode() const;

private:
    /// <summary>
    /// コントローラーを初期化
    /// </summary>
    void InitializeControllers();

    /// <summary>
    /// 現在のモードに基づいてコントローラーをアクティブ化
    /// </summary>
    void UpdateActiveController();

private:
    // カメラマネージャー（シングルトン参照）
    CameraManager* cameraManager_ = nullptr;

    // カメラアニメーション
    std::unique_ptr<CameraAnimation> cameraAnimation_;

    // コントローラー参照（高速アクセス用）
    FirstPersonController* firstPersonController_ = nullptr;
    TopDownController* topDownController_ = nullptr;

    // ターゲット情報
    const Transform* primaryTarget_ = nullptr;
    const Transform* secondaryTarget_ = nullptr;

    // モード管理
    bool isFirstPersonMode_ = false;

    // カメラ参照（高速アクセス用）
    Camera* camera_ = nullptr;
};