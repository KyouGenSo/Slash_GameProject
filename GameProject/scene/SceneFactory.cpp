#include "SceneFactory.h"

#include "ResultScene.h"
#include "TitleScene.h"
#include "GameScene.h"

#ifdef _DEBUG
#include "DebugUIManager.h"
#endif

using namespace Tako;

namespace {
  struct SceneEntry {
    const char* name;
    std::unique_ptr<BaseScene> (*create)();
  };

  const SceneEntry kScenes[] = {
    { "title", []() -> std::unique_ptr<BaseScene> { return std::make_unique<TitleScene>(); } },
    { "game",  []() -> std::unique_ptr<BaseScene> { return std::make_unique<GameScene>(); } },
    { "clear", []() -> std::unique_ptr<BaseScene> { return std::make_unique<ResultScene>("gameClear_Text.dds"); } },
    { "over",  []() -> std::unique_ptr<BaseScene> { return std::make_unique<ResultScene>("gameOver_Text.dds"); } },
  };
}

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName)
{
  for (const SceneEntry& entry : kScenes) {
    if (sceneName == entry.name) {
      return entry.create();
    }
  }

#ifdef _DEBUG
  DebugUIManager::GetInstance()->AddLog("Unknown scene name: " + sceneName, DebugUIManager::LogType::Error);
#endif

  return nullptr;
}

std::vector<std::string> SceneFactory::GetSceneNames() const
{
  std::vector<std::string> names;
  for (const SceneEntry& entry : kScenes) {
    names.emplace_back(entry.name);
  }
  return names;
}
