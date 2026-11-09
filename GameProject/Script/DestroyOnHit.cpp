#include "DestroyOnHit.h"
#include "Actor.h"

void DestroyOnHit::OnCollisionEnter(Tako::Actor* other) {
    if (!other || (!otherName_.empty() && other->GetName() != otherName_)) {
        return;
    }
    GetOwner()->Destroy();
}
