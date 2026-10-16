#include "HPBarUI.h"

#include <algorithm>

using namespace Tako;

namespace {
    // レイアウトの要素名。描画順は JSON の並び（Background → Bar2 → Bar）
    constexpr const char* kBackgroundName = "Background";
    constexpr const char* kBarName        = "Bar";
    constexpr const char* kBar2Name       = "Bar2";
    constexpr const char* kElementNames[] = { kBackgroundName, kBar2Name, kBarName };
}

void HPBarUI::Initialize(const std::string& layoutName)
{
    layout_.Load(layoutName);
}

void HPBarUI::Update(float currentValue, float maxValue)
{
    float ratio = (maxValue > 0.0f) ? (currentValue / maxValue) : 0.0f;
    SetFillRatio(kBarName, std::clamp(ratio, 0.0f, 1.0f));
    layout_.Update();
}

void HPBarUI::UpdateDual(float currentHp, float maxHp, float phase2Threshold, uint32_t phase)
{
    if (phase == 1) {
        // フェーズ1: 手前バー(Bar)が HP 減少で縮小、奥バー(Bar2)はフルサイズ
        float phase1Hp = currentHp - phase2Threshold;
        float phase1MaxHp = maxHp - phase2Threshold;
        float phase1Ratio = (phase1MaxHp > 0.0f) ? (phase1Hp / phase1MaxHp) : 0.0f;

        SetFillRatio(kBarName, std::clamp(phase1Ratio, 0.0f, 1.0f));
        SetFillRatio(kBar2Name, 1.0f);
    }
    else {
        // フェーズ2: 手前バーは0幅、奥バーが HP 減少で縮小
        float phase2Ratio = (phase2Threshold > 0.0f) ? (currentHp / phase2Threshold) : 0.0f;

        SetFillRatio(kBarName, 0.0f);
        SetFillRatio(kBar2Name, std::clamp(phase2Ratio, 0.0f, 1.0f));
    }
    layout_.Update();
}

void HPBarUI::Draw()
{
    layout_.Draw();
}

void HPBarUI::SetPosition(float screenXRatio, float screenYRatio)
{
    for (const char* name : kElementNames) {
        layout_.SetAnchor(name, { screenXRatio, screenYRatio });
    }
}

void HPBarUI::SetBarColor(const Vector4& color)
{
    layout_.SetColor(kBarName, color);
}

void HPBarUI::SetAnchorPoint(const Vector2& anchor)
{
    for (const char* name : kElementNames) {
        layout_.SetPivot(name, anchor);
    }
}

void HPBarUI::SetFillRatio(const char* barName, float ratio)
{
    // 満タンの幅はゲームが書き換えない Background から取る（UI Editor で幅を変えるとそのまま反映される）
    layout_.SetSize(barName, { layout_.GetSize(kBackgroundName).x * ratio, layout_.GetSize(barName).y });
}
