#pragma once
#include "BaseScene.h"
#include"Object3d.h"
#include "EmitterManager.h"
#include "Sprite.h"
#include "SkyBox.h"
#include "BoneTracker.h"
#include "EmitterManager.h"
#include "FollowCamera/followCamera.h"
#include "Object/Player/Player.h"
#include "Object/Boss/Boss.h"

/// <summary>
/// ゲームメインシーンクラス
/// プレイヤーとボスの戦闘、ゲームプレイの中核を管理
/// </summary>
class GameScene : public BaseScene
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
  /// ImGuiの描画
  /// </summary>
  void DrawImGui() override;

private: // メンバ変数

	std::unique_ptr<SkyBox> skyBox_;  ///< スカイボックス（環境マップ）

	std::unique_ptr<Object3d> ground_;  ///< 地面オブジェクト

	std::unique_ptr<Player> player_;  ///< プレイヤーキャラクター

	std::unique_ptr<Boss> boss_;  ///< ボスキャラクター

	std::unique_ptr<FollowCamera> followCamera_;  ///< プレイヤー追従カメラ

	Transform groundUvTransform_{};  ///< 地面のUVトランスフォーム（テクスチャスクロール等に使用）

	std::unique_ptr<EmitterManager> emitterManager_;  ///< パーティクルエミッター管理

	std::unique_ptr<Sprite> toTitleText_;  ///< タイトルに戻るボタンテキスト

	bool isDebug_ = false;  ///< デバッグモードフラグ
};
