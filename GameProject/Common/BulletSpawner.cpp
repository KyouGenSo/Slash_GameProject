#include "BulletSpawner.h"

using namespace Tako;

void BulletSpawner::RequestSpawn(const Vector3& position, const Vector3& velocity)
{
    pendingBullets_.push_back({ position, velocity });
}

std::vector<BulletSpawnRequest> BulletSpawner::Consume()
{
    auto result = std::move(pendingBullets_);
    pendingBullets_.clear();
    return result;
}
