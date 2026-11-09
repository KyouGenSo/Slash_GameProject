#include "Mover.h"
#include "Actor.h"

#include <cmath>
#include <numbers>

void Mover::BeginPlay() {
    origin_ = GetOwner()->GetTransform().translate;
    time_   = 0.0f;
}

void Mover::Tick(float deltaTime) {
    time_ += deltaTime;
    // 0 → 1 → 0 と動く（1 - cos の半分）
    const float t = (1.0f - std::cos(2.0f * std::numbers::pi_v<float> * time_ / period_)) * 0.5f;
    GetOwner()->GetTransform().translate = origin_ + offset_ * t;
}
