#pragma once
#include"TakoFramework.h"
#include"Vector2.h"

/// <summary>
/// 3Dアクションゲームのメインクラス
/// ゲーム全体の初期化、更新、描画処理を管理する
/// </summary>
class MyGame : public Tako::TakoFramework
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

private: // プライベートメンバ関数

    /// <summary>
    /// GlobalVariablesにパラメータを登録
    /// </summary>
    void RegisterGlobalVariables();

    /// <summary>
    /// 入力関連パラメータを登録
    /// </summary>
    void RegisterInputVariables();

    /// <summary>
    /// ゲームシーン関連パラメータを登録
    /// </summary>
    void RegisterGameSceneVariables();

    /// <summary>
    /// プレイヤー関連パラメータを登録
    /// </summary>
    void RegisterPlayerVariables();

    /// <summary>
    /// ボス関連パラメータを登録
    /// </summary>
    void RegisterBossVariables();

    /// <summary>
    /// 弾丸関連パラメータを登録
    /// </summary>
    void RegisterProjectileVariables();

    /// <summary>
    /// 状態関連パラメータを登録
    /// </summary>
    void RegisterStateVariables();

    /// <summary>
    /// テクスチャリソースの読み込み
    /// </summary>
    void LoadTextrue();

private: // メンバ変数

    uint32_t spriteBasicOnresizeId_ = 0;
};