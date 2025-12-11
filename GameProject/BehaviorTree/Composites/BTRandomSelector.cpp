#include "BTRandomSelector.h"
#include "RandomEngine.h"
#include <algorithm>

using namespace Tako;

BTRandomSelector::BTRandomSelector() {
    name_ = "RandomSelector";
}

BTNodeStatus BTRandomSelector::Execute(BTBlackboard* blackboard) {
    if (children_.empty()) {
        return BTNodeStatus::Failure;
    }

    // 新しい選択サイクルの開始時のみシャッフル
    if (needsShuffle_) {
        ShuffleIndices();
        needsShuffle_ = false;
        currentShuffledIdx_ = 0;
    }

    // シャッフル順で実行（前回のRunning位置から継続）
    for (size_t i = currentShuffledIdx_; i < shuffledIndices_.size(); ++i) {
        size_t idx = shuffledIndices_[i];
        BTNodeStatus childStatus = children_[idx]->Execute(blackboard);

        if (childStatus == BTNodeStatus::Success) {
            needsShuffle_ = true;  // 次回はシャッフル
            status_ = BTNodeStatus::Success;
            return status_;
        }
        else if (childStatus == BTNodeStatus::Running) {
            currentShuffledIdx_ = i;  // 現在位置を記憶（シャッフルしない）
            status_ = BTNodeStatus::Running;
            return status_;
        }
        // Failureの場合は次へ
    }

    // 全て失敗
    needsShuffle_ = true;  // 次回はシャッフル
    status_ = BTNodeStatus::Failure;
    return status_;
}

void BTRandomSelector::Reset() {
    BTNode::Reset();
    currentShuffledIdx_ = 0;
    needsShuffle_ = true;
}

void BTRandomSelector::ShuffleIndices() {
    shuffledIndices_.resize(children_.size());
    for (size_t i = 0; i < children_.size(); ++i) {
        shuffledIndices_[i] = i;
    }

    // Fisher-Yatesシャッフル（RandomEngine使用）
    RandomEngine* rng = RandomEngine::GetInstance();
    for (size_t i = shuffledIndices_.size() - 1; i > 0; --i) {
        size_t j = static_cast<size_t>(rng->GetInt(0, static_cast<int>(i)));
        std::swap(shuffledIndices_[i], shuffledIndices_[j]);
    }
}
