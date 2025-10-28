#pragma once
#include "CameraAnimation/CameraAnimation.h"
#include "CameraAnimation/CameraKeyframe.h"
#include <vector>
#include <memory>
#include <string>

#ifdef _DEBUG

/// <summary>
/// カメラアニメーションの編集履歴管理
/// アンドゥ/リドゥ機能を提供
/// </summary>
class CameraAnimationHistory {
public:
    /// <summary>
    /// 操作タイプ
    /// </summary>
    enum class ActionType {
        ADD_KEYFRAME,       ///< キーフレーム追加
        DELETE_KEYFRAME,    ///< キーフレーム削除
        EDIT_KEYFRAME,      ///< キーフレーム編集
        MOVE_KEYFRAME,      ///< キーフレーム移動
        BULK_EDIT,          ///< 一括編集
        CLEAR_ALL           ///< 全消去
    };

    /// <summary>
    /// 編集アクションの基底クラス
    /// </summary>
    class Action {
    public:
        virtual ~Action() = default;

        /// <summary>
        /// アクションの実行
        /// </summary>
        virtual void Execute(CameraAnimation* animation) = 0;

        /// <summary>
        /// アクションの取り消し
        /// </summary>
        virtual void Undo(CameraAnimation* animation) = 0;

        /// <summary>
        /// アクションのタイプを取得
        /// </summary>
        virtual ActionType GetType() const = 0;

        /// <summary>
        /// アクションの説明を取得
        /// </summary>
        virtual std::string GetDescription() const = 0;
    };

    /// <summary>
    /// キーフレーム追加アクション
    /// </summary>
    class AddKeyframeAction : public Action {
    public:
        AddKeyframeAction(const CameraKeyframe& keyframe, size_t index)
            : keyframe_(keyframe), index_(index) {}

        void Execute(CameraAnimation* animation) override;
        void Undo(CameraAnimation* animation) override;
        ActionType GetType() const override { return ActionType::ADD_KEYFRAME; }
        std::string GetDescription() const override { return "Add Keyframe"; }

    private:
        CameraKeyframe keyframe_;
        size_t index_;
    };

    /// <summary>
    /// キーフレーム削除アクション
    /// </summary>
    class DeleteKeyframeAction : public Action {
    public:
        DeleteKeyframeAction(const CameraKeyframe& keyframe, size_t index)
            : keyframe_(keyframe), index_(index) {}

        void Execute(CameraAnimation* animation) override;
        void Undo(CameraAnimation* animation) override;
        ActionType GetType() const override { return ActionType::DELETE_KEYFRAME; }
        std::string GetDescription() const override { return "Delete Keyframe"; }

    private:
        CameraKeyframe keyframe_;
        size_t index_;
    };

    /// <summary>
    /// キーフレーム編集アクション
    /// </summary>
    class EditKeyframeAction : public Action {
    public:
        EditKeyframeAction(size_t index, const CameraKeyframe& oldKf, const CameraKeyframe& newKf)
            : index_(index), oldKeyframe_(oldKf), newKeyframe_(newKf) {}

        void Execute(CameraAnimation* animation) override;
        void Undo(CameraAnimation* animation) override;
        ActionType GetType() const override { return ActionType::EDIT_KEYFRAME; }
        std::string GetDescription() const override { return "Edit Keyframe"; }

    private:
        size_t index_;
        CameraKeyframe oldKeyframe_;
        CameraKeyframe newKeyframe_;
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CameraAnimationHistory();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CameraAnimationHistory();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="animation">対象のアニメーション</param>
    void Initialize(CameraAnimation* animation);

    /// <summary>
    /// アクションの記録と実行
    /// </summary>
    void ExecuteAction(std::unique_ptr<Action> action);

    /// <summary>
    /// キーフレーム追加を記録
    /// </summary>
    void RecordAdd(size_t index);

    /// <summary>
    /// キーフレーム削除を記録
    /// </summary>
    void RecordDelete(size_t index, const CameraKeyframe& keyframe);

    /// <summary>
    /// キーフレーム編集を記録
    /// </summary>
    void RecordEdit(size_t index, const CameraKeyframe& oldKf, const CameraKeyframe& newKf);

    /// <summary>
    /// アンドゥ実行
    /// </summary>
    void Undo();

    /// <summary>
    /// リドゥ実行
    /// </summary>
    void Redo();

    /// <summary>
    /// アンドゥ可能か
    /// </summary>
    bool CanUndo() const { return currentIndex_ > 0; }

    /// <summary>
    /// リドゥ可能か
    /// </summary>
    bool CanRedo() const { return currentIndex_ < history_.size(); }

    /// <summary>
    /// 履歴のクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// 最大履歴数の設定
    /// </summary>
    void SetMaxHistorySize(size_t size) { maxHistorySize_ = size; }

    /// <summary>
    /// 現在の履歴数を取得
    /// </summary>
    size_t GetHistorySize() const { return history_.size(); }

    /// <summary>
    /// 現在の履歴インデックスを取得
    /// </summary>
    size_t GetCurrentIndex() const { return currentIndex_; }

    /// <summary>
    /// 履歴情報の取得（デバッグ用）
    /// </summary>
    std::string GetHistoryInfo() const;

private:
    /// <summary>
    /// 履歴サイズの制限
    /// </summary>
    void LimitHistorySize();

private:
    CameraAnimation* animation_ = nullptr;                   ///< 対象アニメーション
    std::vector<std::unique_ptr<Action>> history_;          ///< アクション履歴
    size_t currentIndex_ = 0;                               ///< 現在の履歴位置
    size_t maxHistorySize_ = 100;                           ///< 最大履歴数
    bool isExecuting_ = false;                              ///< アクション実行中フラグ
};

#endif // _DEBUG