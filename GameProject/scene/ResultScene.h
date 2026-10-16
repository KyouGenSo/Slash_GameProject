#pragma once
#include <string>

#include "BaseScene.h"
#include "UILayout.h"
#include "CameraSystem/CameraConfig.h"

/// <summary>
/// クリア/ゲームオーバー結果画面。レイアウト（resources/Json/UI/Result.json）の Title のテクスチャ差し替えで両用途を兼ねる
/// </summary>
class ResultScene : public Tako::BaseScene
{
public: //メンバー関数
    /// <param name="titleTexture">中央に表示するタイトルテクスチャ名</param>
    explicit ResultScene(const std::string& titleTexture);

    void Initialize() override;

    void Finalize() override;

    void Update() override;

    void Draw() override;
    void DrawWithoutEffect() override;

    void DrawImGui() override;

private: //メンバー変数
    std::string titleTexture_;

    // UI
    Tako::UILayout layout_;

    // カメラ非表示 Y 座標
    float cameraHiddenY_ = CameraConfig::HIDDEN_Y;
};
