#include "HPBarUI.h"

#include <algorithm>
#include "WinApp.h"

using namespace Tako;

void HPBarUI::Initialize(
    const std::string& texture,
    const Vector2& size,
    float screenXRatio,
    float screenYRatio,
    const Vector4& barColor,
    const Vector4& bgColor)
{
    baseSize_ = size;
    screenXRatio_ = screenXRatio;
    screenYRatio_ = screenYRatio;
    isDualBar_ = false;

    Vector2 screenPos = CalculateScreenPosition();

    // バースプライトの初期化
    barSprite_ = std::make_unique<Sprite>();
    barSprite_->Initialize(texture);
    barSprite_->SetSize(baseSize_);
    barSprite_->SetAnchorPoint(Vector2(1.0f, 0.0f));  // 右上アンカー
    barSprite_->SetColor(barColor);
    barSprite_->SetPos(screenPos);

    // 背景スプライトの初期化
    bgSprite_ = std::make_unique<Sprite>();
    bgSprite_->Initialize(texture);
    bgSprite_->SetSize(baseSize_);
    bgSprite_->SetAnchorPoint(Vector2(1.0f, 0.0f));
    bgSprite_->SetColor(bgColor);
    bgSprite_->SetPos(screenPos);
}

void HPBarUI::InitializeDual(
    const std::string& texture,
    const Vector2& size,
    float screenXRatio,
    float screenYRatio,
    const Vector4& bar1Color,
    const Vector4& bar2Color,
    const Vector4& bgColor)
{
    baseSize_ = size;
    screenXRatio_ = screenXRatio;
    screenYRatio_ = screenYRatio;
    isDualBar_ = true;

    Vector2 screenPos = CalculateScreenPosition();

    // フェーズ1バースプライトの初期化
    barSprite_ = std::make_unique<Sprite>();
    barSprite_->Initialize(texture);
    barSprite_->SetSize(baseSize_);
    barSprite_->SetAnchorPoint(Vector2(0.0f, 0.0f));  // 左上アンカー
    barSprite_->SetColor(bar1Color);
    barSprite_->SetPos(screenPos);

    // フェーズ2バースプライトの初期化
    bar2Sprite_ = std::make_unique<Sprite>();
    bar2Sprite_->Initialize(texture);
    bar2Sprite_->SetSize(baseSize_);
    bar2Sprite_->SetAnchorPoint(Vector2(0.0f, 0.0f));
    bar2Sprite_->SetColor(bar2Color);
    bar2Sprite_->SetPos(screenPos);

    // 背景スプライトの初期化
    bgSprite_ = std::make_unique<Sprite>();
    bgSprite_->Initialize(texture);
    bgSprite_->SetSize(baseSize_);
    bgSprite_->SetAnchorPoint(Vector2(0.0f, 0.0f));
    bgSprite_->SetColor(bgColor);
    bgSprite_->SetPos(screenPos);
}

void HPBarUI::Update(float currentValue, float maxValue)
{
    if (!barSprite_ || !bgSprite_) return;

    float ratio = (maxValue > 0.0f) ? (currentValue / maxValue) : 0.0f;
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    Vector2 screenPos = CalculateScreenPosition();

    // バーのサイズを更新
    barSprite_->SetSize(Vector2(baseSize_.x * ratio, baseSize_.y));
    barSprite_->SetPos(screenPos);
    barSprite_->Update();

    bgSprite_->SetPos(screenPos);
    bgSprite_->Update();
}

void HPBarUI::UpdateDual(float currentHp, float maxHp, float phase2Threshold)
{
    if (!barSprite_ || !bar2Sprite_ || !bgSprite_) return;

    Vector2 screenPos = CalculateScreenPosition();

    // フェーズ1のHP（phase2Threshold以上の部分）
    float phase1MaxHp = maxHp - phase2Threshold;
    float phase1CurrentHp = std::max<float>(0.0f, currentHp - phase2Threshold);
    float phase1Ratio = (phase1MaxHp > 0.0f) ? (phase1CurrentHp / phase1MaxHp) : 0.0f;
    phase1Ratio = std::clamp(phase1Ratio, 0.0f, 1.0f);

    // フェーズ2のHP（phase2Threshold以下の部分）
    float phase2CurrentHp = std::min<float>(currentHp, phase2Threshold);
    float phase2Ratio = (phase2Threshold > 0.0f) ? (phase2CurrentHp / phase2Threshold) : 0.0f;
    phase2Ratio = std::clamp(phase2Ratio, 0.0f, 1.0f);

    // フェーズ1バー（全体の半分を使用）
    float halfWidth = baseSize_.x * 0.5f;
    barSprite_->SetSize(Vector2(halfWidth * phase1Ratio, baseSize_.y));
    barSprite_->SetPos(Vector2(screenPos.x + halfWidth, screenPos.y));
    barSprite_->Update();

    // フェーズ2バー（全体の半分を使用）
    bar2Sprite_->SetSize(Vector2(halfWidth * phase2Ratio, baseSize_.y));
    bar2Sprite_->SetPos(screenPos);
    bar2Sprite_->Update();

    bgSprite_->SetPos(screenPos);
    bgSprite_->Update();
}

void HPBarUI::Draw()
{
    if (bgSprite_) bgSprite_->Draw();
    if (bar2Sprite_ && isDualBar_) bar2Sprite_->Draw();
    if (barSprite_) barSprite_->Draw();
}

void HPBarUI::SetPosition(float screenXRatio, float screenYRatio)
{
    screenXRatio_ = screenXRatio;
    screenYRatio_ = screenYRatio;
}

void HPBarUI::SetBarColor(const Vector4& color)
{
    if (barSprite_) barSprite_->SetColor(color);
}

void HPBarUI::SetAnchorPoint(const Vector2& anchor)
{
    if (barSprite_) barSprite_->SetAnchorPoint(anchor);
    if (bar2Sprite_) bar2Sprite_->SetAnchorPoint(anchor);
    if (bgSprite_) bgSprite_->SetAnchorPoint(anchor);
}

Vector2 HPBarUI::CalculateScreenPosition() const
{
    return Vector2(
        WinApp::clientWidth * screenXRatio_,
        WinApp::clientHeight * screenYRatio_);
}
