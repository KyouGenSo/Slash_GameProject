#include "Follow.h"
#include "Actor.h"

#include <algorithm>

void Follow::Tick(float deltaTime) {
    const Tako::Actor* target = GetOwner()->Resolve(target_);
    if (!target) {
        return;
    }
    const Tako::Vector3 position = GetOwner()->GetWorldPosition();
    const Tako::Vector3 toTarget = target->GetWorldPosition() - position;
    const float         distance = toTarget.Length();
    if (distance <= stopDistance_) {
        return;
    }
    const float step = (std::min)(speed_ * deltaTime, distance - stopDistance_);
    GetOwner()->SetWorldPosition(position + toTarget * (step / distance));
}
