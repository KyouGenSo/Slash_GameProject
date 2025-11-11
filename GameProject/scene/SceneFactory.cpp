#include "SceneFactory.h"

#include "ClearScene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "OverScene.h"

#ifdef _DEBUG
#include "DebugUIManager.h"
#endif

BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
  BaseScene* newScene = nullptr;

  if (sceneName == "title") {
    newScene = new TitleScene();
  } else if (sceneName == "game") {
    newScene = new GameScene();
  } else if (sceneName == "clear") {
      newScene = new ClearScene();
  } else if (sceneName == "over") {
      newScene = new OverScene();
  } else {
#ifdef _DEBUG
    DebugUIManager::GetInstance()->AddLog("Unknown scene name: " + sceneName, DebugUIManager::LogType::Error);
#endif
  }

  return newScene;
}
