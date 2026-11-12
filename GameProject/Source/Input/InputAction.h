// 自動生成（Engine Settings の Save で上書きされるので直接編集しない）
#pragma once
#include <string_view>

/// <summary>
/// 入力アクション名（Engine Settings > Input > Actions。Input::TriggerAction などに渡す）
/// </summary>
namespace InputAction {
    inline constexpr std::string_view Attack = "Attack";
    inline constexpr std::string_view Dash = "Dash";
    inline constexpr std::string_view Parry = "Parry";
    inline constexpr std::string_view Pause = "Pause";
}
