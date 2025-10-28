// カメラアニメーションFOV自動復元機能の使用例
// このファイルはFOV復元機能の動作を説明するサンプルです

#include "CameraAnimation/CameraAnimation.h"
#include "CameraSystem/CameraAnimationController.h"

/// <summary>
/// FOV自動復元機能の動作例
/// </summary>
void DemonstrateFOVRestoration() {

    // カメラとアニメーションのセットアップ
    Camera* camera = /* カメラインスタンス */;
    CameraAnimationController* controller = /* コントローラーインスタンス */;
    CameraAnimation* animation = controller->GetCurrentAnimation();

    // ゲームプレイ中の標準FOV（例：60度）
    float standardGameplayFov = 60.0f;
    camera->SetFovY(standardGameplayFov);

    //=====================================
    // 例1: ズームインアニメーション
    //=====================================
    {
        // キーフレーム追加（FOVを徐々に狭める = ズームイン）
        CameraKeyframe kf1, kf2, kf3;

        kf1.time = 0.0f;
        kf1.fov = 60.0f;  // 開始時の標準FOV

        kf2.time = 2.0f;
        kf2.fov = 30.0f;  // ズームイン（望遠効果）

        kf3.time = 4.0f;
        kf3.fov = 45.0f;  // 少し戻す

        animation->AddKeyframe(kf1);
        animation->AddKeyframe(kf2);
        animation->AddKeyframe(kf3);

        // アニメーション再生
        animation->Play();
        // このとき、内部で originalFov_ = 60.0f が保存される

        // アニメーション終了後（4秒後）
        // - 通常終了時：自動的にFOVが60度に戻る
        // - Stop()呼び出し時：即座にFOVが60度に戻る
        // - カメラの位置と回転はアニメーション終了時のまま
    }

    //=====================================
    // 例2: カットシーンでの動的FOV変化
    //=====================================
    {
        // プレイヤーのゲームプレイ中のFOV
        camera->SetFovY(75.0f);  // 広角設定でプレイ中

        // カットシーン開始
        animation->Play();  // originalFov_ = 75.0f が保存

        // カットシーン中はキーフレームに従ってFOVが変化
        // - 敵の登場シーン：FOV 40（ズーム効果）
        // - バトル開始：FOV 90（広角で迫力演出）
        // - 決めポーズ：FOV 50（標準的な映画的視野）

        // カットシーン終了後
        // FOVは自動的に75度（プレイヤーの設定）に戻る
        // → プレイヤーは違和感なくゲームプレイを再開できる
    }

    //=====================================
    // 例3: ループアニメーションとFOV復元
    //=====================================
    {
        animation->SetLooping(true);
        animation->Play();  // originalFov_ = 現在のFOV が保存

        // ループ中はFOV復元されない
        // アニメーションのFOV変化が繰り返される

        animation->Stop();  // 手動停止
        // ここでFOVが元の値に復元される
    }

    //=====================================
    // 例4: 複数回Play()を呼んだ場合
    //=====================================
    {
        camera->SetFovY(60.0f);
        animation->Play();  // originalFov_ = 60.0f

        // 何らかの理由で再度Play()を呼ぶ
        camera->SetFovY(80.0f);  // 途中でFOVが変更された
        animation->Play();  // originalFov_ = 80.0f に更新

        animation->Stop();
        // FOVは80度に復元される（最後のPlay()時の値）
    }
}

/// <summary>
/// FOV復元の確認方法
/// </summary>
void VerifyFOVRestoration() {
    Camera* camera = /* カメラインスタンス */;
    CameraAnimation* animation = /* アニメーションインスタンス */;

    // 1. 開始前のFOVを記録
    float beforeFov = camera->GetFovY();
    printf("Before animation: FOV = %.1f\n", beforeFov);

    // 2. アニメーション再生
    animation->Play();

    // 3. アニメーション中のFOV確認（Update内で）
    // animation->Update(deltaTime);
    printf("During animation: FOV = %.1f\n", camera->GetFovY());

    // 4. 終了後のFOV確認
    animation->Stop();  // または自然終了を待つ
    float afterFov = camera->GetFovY();
    printf("After animation: FOV = %.1f\n", afterFov);

    // 5. 検証
    if (std::abs(beforeFov - afterFov) < 0.01f) {
        printf("✓ FOV successfully restored!\n");
    } else {
        printf("✗ FOV was not restored correctly\n");
    }
}

/// <summary>
/// 実装の特徴と利点
/// </summary>
/*
【FOV復元機能の特徴】

1. **自動復元**
   - アニメーション終了時に自動的にFOVが元に戻る
   - プログラマーが手動で復元する必要がない

2. **選択的復元**
   - FOVのみを復元（位置と回転は維持）
   - カメラの最終位置を保ちつつ、視野角だけを標準化

3. **確実な復元**
   - Stop()呼び出し時も復元
   - 自然終了時も復元
   - 逆再生での終了時も復元

4. **安全性**
   - hasOriginalFov_フラグで二重復元を防止
   - カメラがnullでも安全に動作

【使用場面】

- カットシーン後のゲームプレイ復帰
- ズーム演出後の通常視野への復帰
- プレイヤーのカメラ設定を維持
- VR/ARでの快適な視野角維持

【注意事項】

- ループアニメーション中は復元されない
- Play()を再度呼ぶと新しいFOVで上書きされる
- 位置と回転は復元されない（意図的な設計）
*/