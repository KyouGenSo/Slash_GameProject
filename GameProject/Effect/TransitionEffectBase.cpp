#include "TransitionEffectBase.h"

TransitionEffectBase::TransitionEffectBase(Tako::EmitterManager* emitterManager)
    : emitterManager_(emitterManager)
{
}

void TransitionEffectBase::Reset()
{
    timer_ = 0.0f;
    isPlaying_ = false;
    isComplete_ = false;
}
