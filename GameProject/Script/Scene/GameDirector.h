#pragma once
#include "ActorRef.h"
#include "Script.h"

#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include "Source/Input/InputHandler.h"
#include "Source/Effect/OverEffectManager.h"
#include "Source/Effect/ClearEffectManager.h"
#include "Source/Effect/BossBorderParticleManager.h"
#include "Source/Effect/DashEffectManager.h"
#include "Script/UI/ControllerUI.h"
#include "Script/UI/PauseMenu.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Tako {
    class EmitterManager;
    class ForceFieldManager;
    class UILayoutComponent;
}

class CameraManager;
class CameraAnimationController;
class GameCamera;

/// <summary>
/// ゲームのシーンの進行役。プレイヤーとボスの戦闘を組み立て、入力・ポーズ・勝敗・更新の順番を決める。
/// プレイヤーとボスより先に入力とポーズを決めるので、Order をそれらより小さくしている（カメラよりは後）
/// </summary>
TCLASS(Order = -100)
class GameDirector : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    void BeginPlay() override;

    void EndPlay() override;

    /// <summary>
    /// 入力・ポーズ・勝敗・カメラモード。この後に、プレイヤー、ボスの順で Tick が来る（TCLASS の Order）
    /// </summary>
    void Tick(float deltaTime) override;

    /// <summary>
    /// プレイヤーとボスが動いた後に行うもの（演出）。カメラは GameCamera が同じ LateTick で動かす
    /// </summary>
    void LateTick(float deltaTime) override;

    void Draw() override;

    /// <summary>
    /// ポーズ中も、ポーズのメニューと解除の入力を動かす
    /// </summary>
    bool TickWhilePaused() const override { return true; }

private: //非公開関数
    /// <summary>
    /// フェーズに応じてカメラモードと移動制限を切り替え
    /// </summary>
    void UpdateCameraMode();

    /// <summary>
    /// アニメーション再生中・デバッグカメラ中は入力をリセット
    /// </summary>
    void UpdateInput();

    /// <summary>
    /// 開始演出の終了処理とクリア・オーバーの判定、各演出の開始
    /// </summary>
    void UpdateGameFlow();

    /// <summary>
    /// 各種エフェクトの更新と演出完了時のシーン遷移
    /// </summary>
    void UpdateEffects();

    void InitializeObject3d();


    void InitializeEmitterManager();

    void InitializeEffectManager();


    void CheckPause();

    void UpdatePause();

    /// <summary>
    /// ポーズを切り替える。シーン全体を止め、ぼかしを掛ける / 外す
    /// </summary>
    void SetPaused(bool paused);

private: //メンバー変数
    //エミッター・フォースフィールド
    Tako::ForceFieldManager* forceFieldManager_ = nullptr;  ///< シーン（BaseScene）の物
    Tako::EmitterManager*    emitterManager_    = nullptr;  ///< シーン（BaseScene）の物

    //シーンオブジェクト
    TPROPERTY(Edit, Required, ActorWith = "Player", Tooltip = "Player のスクリプトを付けた Actor")
    Tako::ActorRef playerActor_;

    TPROPERTY(Edit, Required, ActorWith = "Boss", Tooltip = "Boss のスクリプトを付けた Actor")
    Tako::ActorRef bossActor_;
    TPROPERTY(Edit, Required, ActorWith = "GameCamera", Tooltip = "GameCamera のスクリプトを付けた Actor")
    Tako::ActorRef cameraActor_;
    TPROPERTY(Edit, Required, ActorWith = "ControllerUI", Tooltip = "ControllerUI のスクリプトを付けた Actor")
    Tako::ActorRef controllerUiActor_;
    TPROPERTY(Edit, Required, ActorWith = "PauseMenu", Tooltip = "PauseMenu のスクリプトと UILayoutComponent を付けた Actor（ポーズの間だけ出す）")
    Tako::ActorRef pauseMenuActor_;

    //シーンの行き先
    TPROPERTY(Edit, Category = "Scenes", Asset = "Scene", Tooltip = "ゲームオーバーの演出の後に開く")
    std::string overScene_  = "over";
    TPROPERTY(Edit, Category = "Scenes", Asset = "Scene", Tooltip = "クリアの演出の後に開く")
    std::string clearScene_ = "clear";
    TPROPERTY(Edit, Category = "Scenes", Asset = "Scene", Tooltip = "ポーズメニューの「タイトルへ」で開く")
    std::string titleScene_ = "title";

    Player*                       player_       = nullptr;  ///< playerActor_ のスクリプト
    Boss*                         boss_         = nullptr;  ///< bossActor_ のスクリプト
    std::unique_ptr<InputHandler> inputHandler_;

    //カメラ
    CameraManager*             cameraManager_       = nullptr;
    CameraAnimationController* animationController_ = nullptr;
    GameCamera*                gameCamera_          = nullptr;  ///< cameraActor_ のスクリプト
    bool                       cameraMode_          = false;    ///< true: ThirdPerson, false: TopDown

    //状態
    bool isStart_ = false;

    //エフェクト管理
    std::unique_ptr<OverEffectManager>         overEffectManager_;
    std::unique_ptr<ClearEffectManager>        clearEffectManager_;
    std::unique_ptr<BossBorderParticleManager> bossBorderManager_;
    std::unique_ptr<DashEffectManager>         dashEffectManager_;

    //UI・ポーズ
    ControllerUI*            controllerUI_    = nullptr;  ///< controllerUiActor_ のスクリプト
    PauseMenu*               pauseMenu_       = nullptr;  ///< pauseMenuActor_ のスクリプト
    Tako::UILayoutComponent* pauseMenuLayout_ = nullptr;  ///< pauseMenuActor_ のコンポーネント
    bool                     isPaused_        = false;
    uint32_t                 pauseBlurId_     = 0;        ///< ポーズ中に掛けるブラーのポストエフェクト
};
