#include "CameraManager.h"
#include <algorithm>
#include <sstream>

// シングルトンインスタンス
CameraManager* CameraManager::instance_ = nullptr;

CameraManager* CameraManager::GetInstance() {
    if (!instance_) {
        instance_ = new CameraManager();
    }
    return instance_;
}

void CameraManager::Initialize(Camera* camera) {
    camera_ = camera;
    controllers_.clear();
    nameToIndex_.clear();
    cachedActiveIndex_ = -1;
    cacheValid_ = false;
    needsSort_ = false;
}

void CameraManager::Finalize() {
    DeactivateAllControllers();
    controllers_.clear();
    nameToIndex_.clear();
    camera_ = nullptr;

    // シングルトンインスタンスを削除
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

void CameraManager::Update(float deltaTime) {
    if (!camera_) {
        return;
    }

    // ソートが必要な場合は実行
    if (needsSort_) {
        SortControllersByPriority();
    }

    // 最高優先度のアクティブなコントローラーを取得
    ICameraController* activeController = GetActiveController();
    if (activeController) {
        activeController->Update(deltaTime);
    }
}

void CameraManager::RegisterController(const std::string& name,
                                      std::unique_ptr<ICameraController> controller) {
    if (!controller) {
        return;
    }

    // 既存のコントローラーを削除
    RemoveController(name);

    // カメラを設定
    controller->SetCamera(camera_);

    // 新しいエントリを追加
    size_t newIndex = controllers_.size();
    controllers_.push_back({name, std::move(controller)});
    nameToIndex_[name] = newIndex;

    // ソートが必要
    needsSort_ = true;
    cacheValid_ = false;
}

ICameraController* CameraManager::GetController(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end() && it->second < controllers_.size()) {
        return controllers_[it->second].controller.get();
    }
    return nullptr;
}

bool CameraManager::RemoveController(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it == nameToIndex_.end()) {
        return false;
    }

    size_t indexToRemove = it->second;

    // コントローラーを削除
    controllers_.erase(controllers_.begin() + indexToRemove);
    nameToIndex_.erase(it);

    // インデックスマップを更新
    nameToIndex_.clear();
    for (size_t i = 0; i < controllers_.size(); ++i) {
        nameToIndex_[controllers_[i].name] = i;
    }

    cacheValid_ = false;
    return true;
}

bool CameraManager::ActivateController(const std::string& name) {
    ICameraController* controller = GetController(name);
    if (controller) {
        controller->Activate();
        cacheValid_ = false;
        return true;
    }
    return false;
}

bool CameraManager::DeactivateController(const std::string& name) {
    ICameraController* controller = GetController(name);
    if (controller) {
        controller->Deactivate();
        cacheValid_ = false;
        return true;
    }
    return false;
}

void CameraManager::DeactivateAllControllers() {
    for (auto& entry : controllers_) {
        entry.controller->Deactivate();
    }
    cacheValid_ = false;
}

ICameraController* CameraManager::GetActiveController() const {
    if (!cacheValid_) {
        cachedActiveIndex_ = FindHighestPriorityActiveController();
        cacheValid_ = true;
    }

    if (cachedActiveIndex_ >= 0 &&
        cachedActiveIndex_ < static_cast<int>(controllers_.size())) {
        return controllers_[cachedActiveIndex_].controller.get();
    }

    return nullptr;
}

std::string CameraManager::GetActiveControllerName() const {
    if (!cacheValid_) {
        cachedActiveIndex_ = FindHighestPriorityActiveController();
        cacheValid_ = true;
    }

    if (cachedActiveIndex_ >= 0 &&
        cachedActiveIndex_ < static_cast<int>(controllers_.size())) {
        return controllers_[cachedActiveIndex_].name;
    }

    return "";
}

std::string CameraManager::GetDebugInfo() const {
    std::stringstream ss;
    ss << "=== Camera Manager Debug Info ===" << '\n';
    ss << "Total Controllers: " << controllers_.size() << '\n';
    ss << "Active Controller: " << GetActiveControllerName() << '\n';
    ss << '\n';

    ss << "Controller List (Priority Order):" << '\n';
    for (const auto& entry : controllers_) {
        ss << "  - " << entry.name;
        ss << " [Priority: " << static_cast<int>(entry.controller->GetPriority()) << "]";
        ss << " [Active: " << (entry.controller->IsActive() ? "Yes" : "No") << "]";
        ss << '\n';
    }

    return ss.str();
}

void CameraManager::SortControllersByPriority() {
    // 優先度でソート（降順）
    std::sort(controllers_.begin(), controllers_.end());

    // インデックスマップを再構築
    nameToIndex_.clear();
    for (size_t i = 0; i < controllers_.size(); ++i) {
        nameToIndex_[controllers_[i].name] = i;
    }

    needsSort_ = false;
    cacheValid_ = false;
}

int CameraManager::FindHighestPriorityActiveController() const {
    // 既にソート済みなので、最初に見つかったアクティブなコントローラーが最高優先度
    for (size_t i = 0; i < controllers_.size(); ++i) {
        if (controllers_[i].controller->IsActive()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}