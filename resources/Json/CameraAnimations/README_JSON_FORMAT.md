# カメラアニメーション JSON形式仕様書

## 概要
カメラアニメーションシステムはJSONファイルでアニメーションデータを保存・読み込みできます。
このドキュメントはJSON形式の仕様と後方互換性について説明します。

## JSON形式（v2.0 - 開始モード対応版）

```json
{
    "animation_name": "アニメーション名",
    "duration": 総時間（秒）,
    "loop": ループフラグ（true/false）,
    "play_speed": 再生速度（1.0が標準）,
    "start_mode": 開始モード（0 or 1）,        // v2.0で追加
    "blend_duration": ブレンド時間（秒）,      // v2.0で追加
    "keyframes": [
        {
            "time": キーフレーム時刻,
            "position": { "x": X座標, "y": Y座標, "z": Z座標 },
            "rotation": { "x": X回転, "y": Y回転, "z": Z回転 },
            "fov": 視野角,
            "interpolation": "補間タイプ",
            "coordinateType": "座標系タイプ"
        }
    ]
}
```

## フィールド詳細

### 基本設定
| フィールド | 型 | 説明 | デフォルト値 |
|-----------|-----|------|-------------|
| animation_name | string | アニメーションの名前 | "Untitled" |
| duration | float | 総時間（秒）※自動計算 | - |
| loop | boolean | ループ再生するか | false |
| play_speed | float | 再生速度（1.0が標準） | 1.0 |

### 開始モード設定（v2.0新規）
| フィールド | 型 | 説明 | デフォルト値 |
|-----------|-----|------|-------------|
| start_mode | integer | 開始モード（下記参照） | 0 |
| blend_duration | float | ブレンド時間（秒） | 0.5 |

#### start_mode の値
- `0`: JUMP_CUT - 即座に最初のキーフレームにジャンプ
- `1`: SMOOTH_BLEND - 現在のカメラ位置から滑らかに補間

### キーフレーム設定
| フィールド | 型 | 説明 |
|-----------|-----|------|
| time | float | キーフレームの時刻（秒） |
| position | object | 位置座標 {x, y, z} |
| rotation | object | 回転（ラジアン） {x, y, z} |
| fov | float | 視野角（度） |
| interpolation | string | 補間タイプ（下記参照） |
| coordinateType | string | 座標系タイプ（下記参照） |

#### interpolation の値
- `"LINEAR"`: 線形補間
- `"EASE_IN"`: イーズイン
- `"EASE_OUT"`: イーズアウト
- `"EASE_IN_OUT"`: イーズインアウト

#### coordinateType の値
- `"WORLD"`: ワールド座標系
- `"TARGET_RELATIVE"`: ターゲット相対座標系

## 後方互換性

### v1.0形式（開始モードなし）からの読み込み
```json
{
    "animation_name": "Legacy Animation",
    "duration": 5.0,
    "loop": false,
    "play_speed": 1.0,
    // start_mode と blend_duration が存在しない
    "keyframes": [...]
}
```

v1.0形式のファイルを読み込む場合：
- `start_mode` → `0` (JUMP_CUT) として読み込み
- `blend_duration` → `0.5` として読み込み

これにより、既存のアニメーションファイルも問題なく動作します。

## ファイルの保存場所
- **デフォルトパス**: `resources/Json/CameraAnimations/`
- **ファイル拡張子**: `.json`

## 使用例

### エディターでの保存
1. CameraAnimationEditorでアニメーションを作成
2. Start Mode Settingsで開始モードを設定
3. Save to JSONボタンでファイル保存

### コードでの読み込み
```cpp
CameraAnimationController* controller = /* ... */;
controller->LoadAnimationFromFile("my_animation.json", "MyAnimation");
```

### コードでの保存
```cpp
CameraAnimation* animation = controller->GetAnimation("MyAnimation");
animation->SaveToJson("my_animation.json");
```

## 注意事項
- ファイル名に拡張子が含まれていない場合、自動的に`.json`が追加されます
- ディレクトリが存在しない場合は自動的に作成されます
- 読み込みエラー時はfalseが返され、アニメーションは変更されません
- JSONパースエラーはcatchされ、安全に処理されます

## バージョン履歴
- **v2.0** (2025/01): 開始モード設定（start_mode, blend_duration）を追加
- **v1.1** (以前): coordinateTypeフィールドを追加
- **v1.0** (初期): 基本的なアニメーション機能