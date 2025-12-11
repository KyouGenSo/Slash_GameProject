#pragma once
#include "AbstractSceneFactory.h"
#include <memory>

/// <summary>
/// シーンファクトリークラス
/// シーン名から対応するシーンインスタンスを生成
/// </summary>
class SceneFactory : public Tako::AbstractSceneFactory
{
public: // メンバ関数

	/// <summary>
	/// シーンの生成
	/// </summary>
	/// <param name="sceneName">生成するシーン名</param>
	/// <returns>生成されたシーンインスタンス（生成失敗時はnullptr）</returns>
	std::unique_ptr<Tako::BaseScene> CreateScene(const std::string& sceneName) override;

};
