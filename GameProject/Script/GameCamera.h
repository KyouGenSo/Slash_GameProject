#pragma once
#include "ActorRef.h"
#include "Script.h"

#include "CameraSystem/CameraSettings.h"

class CameraAnimationController;
class ThirdPersonController;
class TopDownController;

/// <summary>
/// ゲームのカメラ。見下ろし・肩越し・カメラアニメーションのコントローラーを CameraManager に登録し、
/// 毎フレームの結果を自分の Actor の Transform と CameraComponent に写す（Debug Viewport とギズモでカメラの動きが見える）。
/// シーンの並びで GameDirector より前に置く（GameDirector が BeginPlay でカメラアニメーションを取りに来るため）
/// </summary>
TCLASS()
class GameCamera : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    void BeginPlay() override;

    void EndPlay() override;

    /// <summary>
    /// プレイヤーとボスが動いた後に、カメラを動かす
    /// </summary>
    void LateTick(float deltaTime) override;

    /// <summary>
    /// Inspector で変えた値を、動いているコントローラーにすぐ渡す
    /// </summary>
    void PostEditChange() override;

#ifdef _DEBUG
    /// <summary>
    /// コントローラーの状態と、カメラアニメーションの操作
    /// </summary>
    bool DrawCustomImGui() override;
#endif

    //=====================================
    //Getter
    //=====================================
    /// <summary>
    /// 開始・クリア・ゲームオーバーの演出を再生する物。BeginPlay の前は nullptr
    /// </summary>
    CameraAnimationController* GetAnimationController() const { return animationController_; }

private: //非公開関数
    void ApplySettings(bool resetsCurrent);

private: //メンバー変数
    TPROPERTY(Edit, Tooltip = "追う相手（プレイヤー）")
    Tako::ActorRef            target_;
    TPROPERTY(Edit, Tooltip = "一緒に画面に入れる相手（ボス）。肩越しではこの相手を向く")
    Tako::ActorRef            secondaryTarget_;
    TPROPERTY(Edit)
    ThirdPersonCameraSettings thirdPerson_;
    TPROPERTY(Edit)
    TopDownCameraSettings     topDown_;

    // CameraManager が持つ
    ThirdPersonController*     thirdPersonController_ = nullptr;
    TopDownController*         topDownController_     = nullptr;
    CameraAnimationController* animationController_   = nullptr;
};
