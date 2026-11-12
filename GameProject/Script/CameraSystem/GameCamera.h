#pragma once
#include "ActorRef.h"
#include "Script.h"

#include "Source/CameraSystem/CameraSettings.h"

#include <string>

class CameraAnimationController;
class ThirdPersonController;
class TopDownController;

/// <summary>
/// ゲームのカメラ。見下ろし・肩越し・カメラアニメーションのコントローラーを CameraManager に登録し、
/// 毎フレームの結果を自分の Actor の Transform と CameraComponent に写す（Debug Viewport とギズモでカメラの動きが見える）。
/// GameDirector が BeginPlay でカメラアニメーションを取りに来るので、Order を GameDirector より小さくしている
/// </summary>
TCLASS(Order = -200, Requires = "CameraComponent")
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

    const std::string& GetClearAnimation() const { return clearAnimation_; }

    const std::string& GetOverAnimation() const { return overAnimation_; }

private: //非公開関数
    void ApplySettings(bool resetsCurrent);

private: //メンバー変数
    TPROPERTY(Edit, Required, Tooltip = "追う相手（プレイヤー）")
    Tako::ActorRef            target_;
    TPROPERTY(Edit, Required, Tooltip = "一緒に画面に入れる相手（ボス）。肩越しではこの相手を向く")
    Tako::ActorRef            secondaryTarget_;
    TPROPERTY(Edit, Category = "Animation", Asset = "CameraAnimation", Tooltip = "ゲームの開始で再生する")
    std::string               startAnimation_ = "game_start";
    TPROPERTY(Edit, Category = "Animation", Asset = "CameraAnimation", Tooltip = "ゲームオーバーで再生する（プレイヤーを基準に動く）")
    std::string               overAnimation_  = "over_anim";
    TPROPERTY(Edit, Category = "Animation", Asset = "CameraAnimation", Tooltip = "クリアで再生する（ボスを基準に動く）")
    std::string               clearAnimation_ = "clear_anim";
    TPROPERTY(Edit)
    ThirdPersonCameraSettings thirdPerson_;
    TPROPERTY(Edit)
    TopDownCameraSettings     topDown_;

    // CameraManager が持つ
    ThirdPersonController*     thirdPersonController_ = nullptr;
    TopDownController*         topDownController_     = nullptr;
    CameraAnimationController* animationController_   = nullptr;
};
