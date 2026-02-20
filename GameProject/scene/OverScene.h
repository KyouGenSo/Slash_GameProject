#pragma once
#include <memory>

#include "BaseScene.h"
#include "vector2.h"
#include "CameraSystem/CameraConfig.h"

namespace Tako {
class Sprite;
}

/// <summary>
/// ゲームオーバーシーンクラス
/// ゲームオーバー時の演出と結果表示を管理
/// </summary>
class OverScene : public Tako::BaseScene
{
public: // メンバ関数

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;
    void DrawWithoutEffect() override;

    /// <summary>
    /// ImGui の描画
    /// </summary>
    void DrawImGui() override;

private: // メンバ変数

    // sprite
    std::unique_ptr<Tako::Sprite> backGround_ = nullptr;
    std::unique_ptr<Tako::Sprite> titleText_ = nullptr;
    std::unique_ptr<Tako::Sprite> pressButtonText_ = nullptr;

    // カメラ非表示 Y 座標
    float cameraHiddenY_ = CameraConfig::HIDDEN_Y;

    // UI 位置・サイズ用変数
    float titleTextHalfWidth_ = 250.0f;  ///< タイトルテキスト半幅（センタリング用）
    float titleTextY_ = 300.0f;  ///< タイトルテキスト Y 座標
    float buttonBottomOffset_ = 300.0f;  ///< ボタン下端からのオフセット
};

