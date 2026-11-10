#pragma once
#include "ActorRef.h"
#include "Script.h"

#include <cstdint>
#include <string>

/// <summary>
/// タイトル画面の進行。同じ Actor の UILayoutComponent のボタンを点滅させ、スタートの入力でタイトル文字のクリップを再生し、
/// 斬撃のパーティクルを増やしながら次のシーンへ移る
/// </summary>
TCLASS()
class TitleScreen : public Tako::Script {
    GENERATED_BODY()

public: //メンバー関数
    void BeginPlay() override;

    void Tick(float deltaTime) override;

#ifdef _DEBUG
    /// <summary>
    /// 演出を試すボタンと、今の進み具合
    /// </summary>
    bool DrawCustomImGui() override;
#endif

private: //非公開関数
    void StartAnimation();

    /// <summary>
    /// タイトル文字を 1 コマ目で止め、斬撃のパーティクルを最初の値に戻す
    /// </summary>
    void ResetAnimation();

    /// <summary>
    /// 斬撃のパーティクルの数と頻度を、進み具合（0〜1）の値にする
    /// </summary>
    void ApplySlash(float progress);

private: //メンバー変数
    TPROPERTY(Edit, Category = "UI", Asset = "AnimationClip", Tooltip = "待っている間、ボタンに再生し続けるクリップ")
    std::string blinkClip_    = "TitleStartBlink";
    TPROPERTY(Edit, Category = "UI", Tooltip = "点滅させる要素の名前")
    std::string blinkElement_ = "StartButton";
    TPROPERTY(Edit, Category = "UI", Asset = "AnimationClip", Tooltip = "スタートの入力で再生するクリップ")
    std::string startClip_    = "TitleSlash";

    TPROPERTY(Edit, Category = "Slash Particle", Tooltip = "演出の間に数と頻度を変える ParticleSystemComponent の Actor")
    Tako::ActorRef slashParticle_;
    TPROPERTY(Edit, Category = "Slash Particle", Tooltip = "演出の長さ（秒）", ClampMin = 0.01, Speed = 0.05)
    float          duration_       = 2.5f;
    TPROPERTY(Edit, Category = "Slash Particle", ClampMin = 1, ClampMax = 300)
    uint32_t       startCount_     = 1;
    TPROPERTY(Edit, Category = "Slash Particle", ClampMin = 1, ClampMax = 300)
    uint32_t       endCount_       = 300;
    TPROPERTY(Edit, Category = "Slash Particle", Tooltip = "出す間隔（秒）", ClampMin = 0.001, ClampMax = 1, Speed = 0.001)
    float          startFrequency_ = 1.0f;
    TPROPERTY(Edit, Category = "Slash Particle", Tooltip = "出す間隔（秒）", ClampMin = 0.001, ClampMax = 1, Speed = 0.001)
    float          endFrequency_   = 0.001f;

    TPROPERTY(Edit, Category = "Transition", Asset = "Scene")
    std::string nextScene_          = "game";
    TPROPERTY(Edit, Category = "Transition", Tooltip = "演出がここまで進んだら次のシーンへ移る", ClampMin = 0, ClampMax = 1, Speed = 0.01)
    float       transitionProgress_ = 0.9f;
    TPROPERTY(Edit, Category = "Transition", Tooltip = "フェードの秒数", ClampMin = 0, Speed = 0.01)
    float       fadeDuration_       = 0.2f;

    bool  isAnimating_ = false;
    float timer_       = 0.0f;
};
