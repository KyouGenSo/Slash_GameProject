#pragma once
#include "AbstractSceneFactory.h"

/// <summary>
/// シーンファクトリークラス
/// シーン名から対応するシーンインスタンスを生成
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public: // メンバ関数

	/// <summary>
	/// シーンの生成
	/// </summary>
	/// <param name="sceneName">生成するシーン名</param>
	/// <returns>生成されたシーンインスタンス（生成失敗時はnullptr）</returns>
	BaseScene* CreateScene(const std::string& sceneName) override;

};
