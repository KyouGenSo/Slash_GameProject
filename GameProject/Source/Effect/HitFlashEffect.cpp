#include "HitFlashEffect.h"
#include "Object3d.h"

using namespace Tako;

void HitFlashEffect::Start(const Vector4& flashColor, float duration)
{
    isActive_ = true;
    timer_ = 0.0f;
    duration_ = duration;
    flashColor_ = flashColor;
}

void HitFlashEffect::Update(float deltaTime, Object3d* target)
{
    if (!isActive_ || !target) return;

    // 点滅中に Start し直しても点滅の色を覚えないよう、始めてから最初の 1 回だけ読む
    if (!hasPrevious_) {
        previousColor_ = target->GetMaterialProperties().GetVector("color");
        hasPrevious_ = true;
    }

    timer_ += deltaTime;

    if (timer_ <= duration_) {
        target->SetMaterialColor(flashColor_);
    }
    else {
        isActive_ = false;
        hasPrevious_ = false;
        target->SetMaterialColor(previousColor_);
    }
}
