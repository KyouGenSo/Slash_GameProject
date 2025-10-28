# FOV復元機能テストガイド

## 修正内容の概要
エディターでアニメーションを切り替えた際にFOVが正しく復元されない問題を修正しました。

### 問題の原因
- CameraAnimationController::SwitchAnimation()で前のアニメーションをStop()することでFOVが復元される
- 新しいアニメーションがその復元済みFOVを「元の値」として記憶してしまう
- 結果として、新しいアニメーション終了時にFOV復元が機能しない

### 解決方法
- `StopWithoutRestore()`メソッドを新規追加
- アニメーション切り替え時はFOV復元をスキップ
- 通常の停止時は従来通りFOVを復元

## テスト手順

### テスト1: エディターでのアニメーション切り替え
1. **CameraAnimationEditorを開く**
2. **Defaultアニメーションを選択して再生**
   - FOVが変化することを確認
3. **再生中にover_animに切り替え**
   - DrawAnimationSelector()でover_animを選択
4. **over_animを再生**
   - FOVが正しく変化することを確認
5. **over_anim終了後**
   - FOVが元の値（ゲーム標準のFOV）に復元されることを確認 ✅

### テスト2: 通常のStop()呼び出し
1. **任意のアニメーションを再生**
2. **Stopボタンを押す**
   - FOVが元の値に復元されることを確認 ✅

### テスト3: 自然終了での復元
1. **ループなしのアニメーションを再生**
2. **最後まで再生させる**
   - FOVが自動的に元の値に復元されることを確認 ✅

### テスト4: ゲームシーンでの動作
1. **GameSceneでアニメーションを再生**
   ```cpp
   animationController_->SwitchAnimation("over_anim");
   animationController_->Play();
   ```
2. **アニメーション終了後**
   - FOVが正しく復元されることを確認 ✅

## 期待される動作

| 操作 | FOV復元 | 備考 |
|------|---------|------|
| エディターでアニメーション切り替え | ❌ 前のアニメーションでは復元しない | StopWithoutRestore()使用 |
| 新しいアニメーション終了 | ✅ 復元される | 正常なStop()またはUpdate()での終了 |
| Stopボタン押下 | ✅ 復元される | 通常のStop()使用 |
| 自然終了（非ループ） | ✅ 復元される | Update()内での復元処理 |

## デバッグ方法

### FOV値の確認
```cpp
// CameraAnimation::Play()内
printf("Play: Original FOV saved: %.1f\n", originalFov_);

// CameraAnimation::Stop()内
printf("Stop: Restoring FOV to: %.1f\n", originalFov_);

// CameraAnimation::StopWithoutRestore()内
printf("StopWithoutRestore: Skipping FOV restoration\n");
```

### エディターでの確認
1. ImGuiウィンドウでカメラのFOV値を表示
2. アニメーション切り替え時のFOV変化を観察
3. hasOriginalFov_フラグの状態を確認

## 注意事項

- **StopWithoutRestore()は内部使用のみ**
  - ユーザーコードでは通常のStop()を使用
  - アニメーションシステム内部でのみ使用

- **FOVのみ復元**
  - カメラの位置と回転は復元されない
  - アニメーション終了位置に留まる

- **複数アニメーション管理**
  - 各アニメーションが独立してFOV復元状態を管理
  - カメラインスタンスは共有される

## 関連ファイル
- `CameraAnimation.h/cpp` - StopWithoutRestore()メソッド追加
- `CameraAnimationController.cpp` - SwitchAnimation()修正
- `CameraAnimationEditor.cpp` - エディターUI（変更なし）