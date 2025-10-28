// カメラアニメーション開始モードの使用例
// このファイルは実際のゲームシーンでの使用方法を示すサンプルです

#include "GameScene.h"
#include "CameraSystem/CameraAnimationController.h"
#include "CameraSystem/CameraManager.h"

/// <summary>
/// カメラアニメーションの開始モード設定例
/// GameScene::Initialize() または適切な場所で使用
/// </summary>
void SetupCameraAnimationStartMode_Example() {

    // CameraManagerからAnimationControllerを取得
    CameraManager* cameraManager = CameraManager::GetInstance();
    auto* animController = dynamic_cast<CameraAnimationController*>(
        cameraManager->GetController("Animation"));

    if (!animController) {
        return;
    }

    //=====================================
    // 例1: Jump Cutモード（即座にジャンプ）
    //=====================================
    {
        // "cutscene_intro"アニメーションをJump Cutモードに設定
        animController->SetAnimationStartModeByName(
            "cutscene_intro",
            CameraAnimation::StartMode::JUMP_CUT,
            0.0f  // Jump Cutモードではブレンド時間は無視される
        );

        // このモードでは、Play()を呼んだ瞬間に最初のキーフレームの
        // カメラ位置・回転・FOVが即座に適用されます
    }

    //=====================================
    // 例2: Smooth Blendモード（滑らかな補間）
    //=====================================
    {
        // "battle_camera"アニメーションをSmooth Blendモードに設定
        animController->SetAnimationStartModeByName(
            "battle_camera",
            CameraAnimation::StartMode::SMOOTH_BLEND,
            1.0f  // 1秒かけてブレンド
        );

        // このモードでは、Play()を呼ぶと現在のカメラ状態から
        // 最初のキーフレームまで1秒かけて滑らかに補間されます
    }

    //=====================================
    // 例3: プレイヤー追従アニメーションの設定
    //=====================================
    {
        // プレイヤーを追従するアニメーションの場合
        Transform* playerTransform = nullptr;  // プレイヤーのTransform

        // アニメーションにターゲットを設定
        animController->SetAnimationTargetByName("over_anim", playerTransform);

        // Smooth Blendモードで開始（プレイヤーへの視点移動を滑らかに）
        animController->SetAnimationStartModeByName(
            "over_anim",
            CameraAnimation::StartMode::SMOOTH_BLEND,
            0.5f  // 0.5秒でブレンド
        );
    }

    //=====================================
    // 例4: 現在のアニメーションのみ設定
    //=====================================
    {
        // 現在選択中のアニメーションの開始モードを設定
        animController->SetAnimationStartMode(
            CameraAnimation::StartMode::SMOOTH_BLEND,
            0.8f
        );
    }

    //=====================================
    // 例5: ゲーム中での動的な切り替え
    //=====================================
    {
        bool isInCombat = false;  // 戦闘中フラグ（例）

        if (isInCombat) {
            // 戦闘中は即座に切り替え
            animController->SetAnimationStartMode(
                CameraAnimation::StartMode::JUMP_CUT
            );
        } else {
            // 非戦闘時は滑らかに切り替え
            animController->SetAnimationStartMode(
                CameraAnimation::StartMode::SMOOTH_BLEND,
                1.5f
            );
        }

        // アニメーション再生
        animController->SwitchAnimation("camera_animation_name");
        animController->Play();
    }
}

/// <summary>
/// ブレンド進行状況の確認例
/// Update()で使用
/// </summary>
void CheckBlendProgress_Example() {
    CameraManager* cameraManager = CameraManager::GetInstance();
    auto* animController = dynamic_cast<CameraAnimationController*>(
        cameraManager->GetController("Animation"));

    if (!animController) {
        return;
    }

    // 現在のアニメーションを取得
    CameraAnimation* currentAnim = animController->GetCurrentAnimation();
    if (!currentAnim) {
        return;
    }

    // ブレンド中かチェック
    if (currentAnim->IsBlending()) {
        float progress = currentAnim->GetBlendProgress();

        // ブレンド進行度に応じた処理
        if (progress < 0.3f) {
            // ブレンド開始直後の処理
            // 例: UIのフェードイン開始
        } else if (progress > 0.7f && progress < 0.8f) {
            // ブレンド終了間近の処理
            // 例: 次のイベントの準備
        }

        // デバッグ表示（開発中のみ）
        #ifdef _DEBUG
        ImGui::Text("Camera Blending: %.1f%%", progress * 100.0f);
        #endif
    }
}

/// <summary>
/// 使用上の注意事項とベストプラクティス
/// </summary>
/*
【Jump Cutモードを使うべき場面】
1. カットシーンの開始時
2. テレポート後のカメラリセット
3. リスポーン時のカメラ位置復帰
4. メニューからゲームに戻る時

【Smooth Blendモードを使うべき場面】
1. ゲームプレイ中のカメラ切り替え
2. プレイヤー操作カメラからシネマティックカメラへの遷移
3. 会話シーンの開始
4. ボス戦などの特殊カメラへの切り替え

【ブレンド時間の目安】
- 0.3秒以下: 素早い切り替え（緊急時、戦闘中）
- 0.5～1.0秒: 標準的な切り替え（通常のゲームプレイ）
- 1.0～2.0秒: ゆったりとした切り替え（演出重視）
- 2.0秒以上: 特殊演出（タイトル画面、エンディング等）

【パフォーマンスの考慮】
- ブレンド中は毎フレーム補間計算が発生します
- 複数のカメラアニメーションを同時にブレンドすることは避けましょう
- ブレンド時間が長すぎるとプレイヤーの操作感を損なう可能性があります
*/