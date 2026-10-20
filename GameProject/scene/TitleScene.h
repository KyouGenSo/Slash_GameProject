#pragma once
#include "BaseScene.h"
#include "UILayout.h"
#include"Object3d.h"
#include "AABB.h"
#include "EmitterManager.h"
#include "ForceFieldManager.h"
#include "PostEffectManager.h"
#include "CameraSystem/CameraConfig.h"
#include <memory>

/// <summary>
/// タイトル画面の演出・UI・ゲーム開始処理を管理
/// </summary>
class TitleScene : public Tako::BaseScene
{
public: //メンバー関数

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;
	void DrawWithoutEffect() override;

	void DrawImGui() override;

	void PlayTitleAnimation();

	void StopTitleAnimation();

	void ResetTitleAnimation();

private: //非公開関数

	// === 初期化系関数 === //

	void InitializeDebugUI();

	void InitializeCamera();

	/// <summary>
	/// ポストエフェクトプロファイル "title" を適用
	/// </summary>
	void InitializePostEffects();

	/// <summary>
	/// 背景・タイトルテキスト・スタートボタンのレイアウト（resources/Json/UI/Title.json）を読み込む
	/// </summary>
	void InitializeUI();

	void InitializeParticles();

	// === 更新系関数 === //

	/// <summary>
	/// タイトルアニメーションと同期しパーティクル発生量を増加
	/// </summary>
	void UpdateSlashParticleAnimation();

	/// <summary>
	/// スペース/A ボタンでゲームシーンへ遷移
	/// </summary>
	void UpdateInput();

private: //メンバー変数
    //エミッター・フォースフィールド
    std::unique_ptr<Tako::EmitterManager>    emitterManager_;
    std::unique_ptr<Tako::ForceFieldManager> forceFieldManager_;

    //UI
    Tako::UILayout layout_;

    float offsetY_ = CameraConfig::HIDDEN_Y;

    //カメラ位置用変数
    float cameraY_ = 9.0f;
    float cameraZ_ = -34.0f;

    //シーン遷移
    float sceneTransitionProgress_ = 0.9f;  ///< シーン遷移トリガーの進行度 0.0-1.0

    //slash パーティクルエミッターアニメーション用変数
    bool     isSlashEmitterAnimating_  = false;
    float    slashEmitterAnimTimer_    = 0.0f;
    float    slashEmitterAnimDuration_ = 2.5f;    ///< 秒
    uint32_t slashEmitterStartCount_   = 1;
    uint32_t slashEmitterEndCount_     = 300;
    float    slashEmitterStartFreq_    = 1.0f;
    float    slashEmitterEndFreq_      = 0.001f;
};
