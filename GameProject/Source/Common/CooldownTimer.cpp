#include "CooldownTimer.h"

#include <algorithm>

void CooldownTimer::Start(float duration)
{
    duration_ = duration;
    remainingTime_ = duration;
}

void CooldownTimer::Update(float deltaTime)
{
    if (remainingTime_ > 0.0f) {
        remainingTime_ -= deltaTime;
        remainingTime_ = std::max(remainingTime_, 0.0f);
    }
}

bool CooldownTimer::IsReady() const
{
    return remainingTime_ <= 0.0f;
}

float CooldownTimer::GetRemainingTime() const
{
    return remainingTime_;
}

float CooldownTimer::GetProgress() const
{
    if (duration_ <= 0.0f) return 1.0f;
    return 1.0f - (remainingTime_ / duration_);
}

void CooldownTimer::Reset()
{
    remainingTime_ = 0.0f;
}
