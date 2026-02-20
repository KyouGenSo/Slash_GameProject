#pragma once
#include <DirectXMath.h>

/// <summary>
/// カメラシステムの設定値を管理
/// マジックナンバーを撲滅し、意味を明確化
/// </summary>
namespace CameraConfig {

    //==================== 共通設定 ====================

    /// <summary>
    /// カメラ追従の滑らかさ（0.0-1.0、値が大きいほど即座に追従）
    /// </summary>
    inline constexpr float FOLLOW_SMOOTHNESS = 0.18f;

    /// <summary>
    /// オフセット補間速度（0.0-1.0）
    /// </summary>
    inline constexpr float OFFSET_LERP_SPEED = 0.08f;

    /// <summary>
    /// 回転補間速度（0.0-1.0）
    /// </summary>
    inline constexpr float ROTATION_LERP_SPEED = 0.15f;

    /// <summary>
    /// 標準 FOV（ラジアン）
    /// </summary>
    inline constexpr float STANDARD_FOV = 0.44999998807907104;

    /// <summary>
    /// ゲームシーンまだ消えないパーティクルが別のシーンでも表示されるのを防ぐため、
    /// カメラを画面外に移動させる際の Y 座標
    /// シーン遷移時などで使用
    /// </summary>
    inline constexpr float HIDDEN_Y = -1000.0f;

    //==================== ThirdPerson 設定 ====================

    namespace ThirdPerson {
        /// <summary>
        /// デフォルトカメラオフセット
        /// </summary>
        inline constexpr float DEFAULT_OFFSET_X = 0.0f;
        inline constexpr float DEFAULT_OFFSET_Y = 2.0f;
        inline constexpr float DEFAULT_OFFSET_Z = -40.0f;

        /// <summary>
    /// ボス注視時の見下ろし角度（度）
    /// </summary>
        inline constexpr float LOOK_DOWN_ANGLE = DirectX::XMConvertToRadians(15.0f);

        /// <summary>
        /// デフォルト X 軸角度（ラジアン）
        /// </summary>
        inline constexpr float DEFAULT_ANGLE_X = DirectX::XMConvertToRadians(8.0f);

        /// <summary>
        /// 回転感度
        /// </summary>
        inline constexpr float DEFAULT_ROTATE_SPEED = 0.05f;

        /// <summary>
        /// ゲームパッド回転感度倍率
        /// </summary>
        inline constexpr float GAMEPAD_ROTATE_MULTIPLIER = 1.0f;
    }

    //==================== TopDown 設定 ====================

    namespace TopDown {
        /// <summary>
        /// 基準カメラ高さ
        /// </summary>
        inline constexpr float BASE_HEIGHT = 10.0f;

        /// <summary>
        /// ターゲット間距離に対する高さ倍率
        /// </summary>
        inline constexpr float HEIGHT_MULTIPLIER = 0.6f;

        /// <summary>
        /// カメラ高さの制限
        /// </summary>
        inline constexpr float MIN_HEIGHT = 26.0f;
        inline constexpr float MAX_HEIGHT = 500.0f;

        /// <summary>
        /// デフォルトカメラ角度（ラジアン）
        /// </summary>
        inline constexpr float DEFAULT_ANGLE_X = DirectX::XMConvertToRadians(25.0f);

        /// <summary>
        /// 後方オフセット設定
        /// </summary>
        inline constexpr float BASE_BACK_OFFSET = -10.0f;
        inline constexpr float BACK_OFFSET_MULTIPLIER = 1.5f;
        inline constexpr float MIN_BACK_OFFSET = -500.0f;
        inline constexpr float MAX_BACK_OFFSET = -52.0f;

        /// <summary>
        /// 初期カメラ高さ
        /// </summary>
        inline constexpr float INITIAL_HEIGHT = 88.0f;

        /// <summary>
        /// 初期後方オフセット
        /// </summary>
        inline constexpr float INITIAL_BACK_OFFSET = -205.0f;
    }

    //==================== アニメーション設定 ====================

    namespace Animation {
        /// <summary>
        /// デフォルト再生速度
        /// </summary>
        inline constexpr float DEFAULT_PLAY_SPEED = 1.0f;

        /// <summary>
        /// キーフレーム予約数
        /// </summary>
        inline constexpr size_t KEYFRAME_RESERVE_COUNT = 32;

        /// <summary>
        /// デフォルト FOV（ラジアン）
        /// </summary>
        inline constexpr float DEFAULT_FOV = 0.45f;

        /// <summary>
        /// デフォルトブレンド時間（秒）
        /// </summary>
        inline constexpr float DEFAULT_BLEND_DURATION = 0.5f;

        /// <summary>
        /// 再生速度最小値
        /// </summary>
        inline constexpr float MIN_PLAY_SPEED = -2.0f;

        /// <summary>
        /// 再生速度最大値
        /// </summary>
        inline constexpr float MAX_PLAY_SPEED = 2.0f;

        /// <summary>
        /// キーフレーム入力ステップ
        /// </summary>
        inline constexpr float KEYFRAME_DRAG_STEP = 0.1f;

        /// <summary>
        /// FOV 最小値（度）
        /// </summary>
        inline constexpr float FOV_MIN_DEGREES = 10.0f;

        /// <summary>
        /// FOV 最大値（度）
        /// </summary>
        inline constexpr float FOV_MAX_DEGREES = 120.0f;

        /// <summary>
        /// デフォルト FOV（度）
        /// </summary>
        inline constexpr float DEFAULT_FOV_DEGREES = 45.0f;
    }

    //==================== カメラシェイク設定 ====================

    namespace Shake {
        /// <summary>
        /// デフォルトシェイク時間（秒）
        /// </summary>
        inline constexpr float DEFAULT_DURATION = 0.3f;

        /// <summary>
        /// デフォルトシェイク強度
        /// </summary>
        inline constexpr float DEFAULT_INTENSITY = 0.5f;
    }
}