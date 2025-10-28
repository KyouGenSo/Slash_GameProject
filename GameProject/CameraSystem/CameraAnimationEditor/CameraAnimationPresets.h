#pragma once
#include "CameraAnimation/CameraKeyframe.h"
#include <vector>
#include <string>
#include <map>

#ifdef _DEBUG

/// <summary>
/// カメラアニメーションのプリセット管理
/// よく使うカメラ動作のテンプレートを提供
/// </summary>
class CameraAnimationPresets {
public:
    /// <summary>
    /// プリセットタイプ
    /// </summary>
    enum class PresetType {
        // 基本動作
        FADE_IN,            ///< フェードイン
        FADE_OUT,           ///< フェードアウト
        ZOOM_IN,            ///< ズームイン
        ZOOM_OUT,           ///< ズームアウト
        PAN_LEFT,           ///< 左パン
        PAN_RIGHT,          ///< 右パン
        TILT_UP,            ///< 上ティルト
        TILT_DOWN,          ///< 下ティルト

        // カメラ動作
        ORBIT_LEFT,         ///< 左回りオービット
        ORBIT_RIGHT,        ///< 右回りオービット
        DOLLY_IN,           ///< ドリーイン
        DOLLY_OUT,          ///< ドリーアウト
        CRANE_UP,           ///< クレーンアップ
        CRANE_DOWN,         ///< クレーンダウン

        // エフェクト
        SHAKE_LIGHT,        ///< 軽い揺れ
        SHAKE_MEDIUM,       ///< 中程度の揺れ
        SHAKE_HEAVY,        ///< 激しい揺れ
        HANDHELD,           ///< 手持ちカメラ風
        SPIRAL_IN,          ///< スパイラルイン
        SPIRAL_OUT,         ///< スパイラルアウト

        // シーン遷移
        CUT_TO_BLACK,       ///< ブラックアウト
        CUT_FROM_BLACK,     ///< ブラックイン
        CROSS_FADE,         ///< クロスフェード
        WIPE_LEFT,          ///< 左ワイプ
        WIPE_RIGHT,         ///< 右ワイプ

        USER_DEFINED        ///< ユーザー定義
    };

    /// <summary>
    /// プリセットデータ
    /// </summary>
    struct Preset {
        std::string name;                          ///< プリセット名
        std::string description;                   ///< 説明
        PresetType type;                           ///< タイプ
        std::vector<CameraKeyframe> keyframes;     ///< キーフレーム配列
        float duration;                             ///< 持続時間
        bool isUserDefined;                        ///< ユーザー定義か
        std::string category;                      ///< カテゴリー
        std::string tags;                          ///< タグ（検索用）
    };

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    CameraAnimationPresets();

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~CameraAnimationPresets();

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// プリセットUIの描画
    /// </summary>
    void Draw();

    /// <summary>
    /// プリセットの取得
    /// </summary>
    /// <param name="type">プリセットタイプ</param>
    /// <returns>プリセットのキーフレーム配列</returns>
    std::vector<CameraKeyframe> GetPreset(PresetType type) const;

    /// <summary>
    /// プリセットの取得（名前指定）
    /// </summary>
    /// <param name="name">プリセット名</param>
    /// <returns>プリセットのキーフレーム配列</returns>
    std::vector<CameraKeyframe> GetPresetByName(const std::string& name) const;

    /// <summary>
    /// カスタムプリセットの追加
    /// </summary>
    /// <param name="name">プリセット名</param>
    /// <param name="keyframes">キーフレーム配列</param>
    void AddCustomPreset(const std::string& name, const std::vector<CameraKeyframe>& keyframes);

    /// <summary>
    /// カスタムプリセットの削除
    /// </summary>
    /// <param name="name">プリセット名</param>
    bool RemoveCustomPreset(const std::string& name);

    /// <summary>
    /// プリセットの保存
    /// </summary>
    /// <param name="filepath">保存先ファイルパス</param>
    bool SavePresets(const std::string& filepath) const;

    /// <summary>
    /// プリセットの読み込み
    /// </summary>
    /// <param name="filepath">読み込みファイルパス</param>
    bool LoadPresets(const std::string& filepath);

    /// <summary>
    /// 選択中のプリセットがあるか
    /// </summary>
    bool HasSelectedPreset() const { return selectedPreset_ != nullptr; }

    /// <summary>
    /// 選択中のプリセットを取得
    /// </summary>
    std::vector<CameraKeyframe> GetSelectedPreset() const;

    /// <summary>
    /// プリセットのカテゴリー一覧を取得
    /// </summary>
    std::vector<std::string> GetCategories() const;

private:
    /// <summary>
    /// ビルトインプリセットの初期化
    /// </summary>
    void InitializeBuiltinPresets();

    /// <summary>
    /// プリセットの生成
    /// </summary>
    std::vector<CameraKeyframe> GeneratePreset(PresetType type) const;

    /// <summary>
    /// フェードインプリセットの生成
    /// </summary>
    std::vector<CameraKeyframe> GenerateFadeIn() const;

    /// <summary>
    /// ズームインプリセットの生成
    /// </summary>
    std::vector<CameraKeyframe> GenerateZoomIn() const;

    /// <summary>
    /// オービットプリセットの生成
    /// </summary>
    std::vector<CameraKeyframe> GenerateOrbit(bool leftDirection) const;

    /// <summary>
    /// カメラシェイクプリセットの生成
    /// </summary>
    std::vector<CameraKeyframe> GenerateShake(float intensity) const;

    /// <summary>
    /// プリセットリストの描画
    /// </summary>
    void DrawPresetList();

    /// <summary>
    /// プリセットプレビューの描画
    /// </summary>
    void DrawPresetPreview();

    /// <summary>
    /// カスタムプリセット作成UIの描画
    /// </summary>
    void DrawCustomPresetCreator();

private:
    // プリセットデータ
    std::map<PresetType, Preset> builtinPresets_;       ///< ビルトインプリセット
    std::map<std::string, Preset> customPresets_;        ///< カスタムプリセット
    Preset* selectedPreset_ = nullptr;                   ///< 選択中のプリセット

    // UI状態
    bool showPreview_ = true;                            ///< プレビュー表示
    char filterText_[128] = "";                          ///< フィルターテキスト
    std::string selectedCategory_ = "All";               ///< 選択中のカテゴリー

    // カスタムプリセット作成用
    char newPresetName_[128] = "";                       ///< 新規プリセット名
    char newPresetDescription_[256] = "";                ///< 新規プリセット説明
    std::vector<CameraKeyframe> tempKeyframes_;          ///< 一時キーフレーム

    // プリセットパラメータ
    float presetDuration_ = 2.0f;                        ///< プリセット持続時間
    float presetIntensity_ = 1.0f;                       ///< プリセット強度
    int presetSteps_ = 10;                               ///< プリセットステップ数
};

#endif // _DEBUG