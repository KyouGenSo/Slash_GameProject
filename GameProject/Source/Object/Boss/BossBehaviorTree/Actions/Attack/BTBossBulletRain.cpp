#include "BTBossBulletRain.h"
#include "Script/Object/Boss/Boss.h"
#include "Source/Object/Boss/Movement/BossAreaBounds.h"
#include "Script/Object/Player/Player.h"
#include "Source/Common/GameConst.h"
#include "RandomEngine.h"

#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

BTBossBulletRain::BTBossBulletRain() {
    name_ = "BossBulletRain";
}

BTBossBulletRain::~BTBossBulletRain() {
    OnCleanup();
}

Tako::BTNodeStatus BTBossBulletRain::OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) {
    if (elapsedTime_ < chargeTime_) {
        bulletSignEffect_.Update(boss, deltaTime);
    }
    else {
        if (bulletSignEffect_.IsActive()) {
            bulletSignEffect_.End(boss);
        }

        // spawnInterval_ ごとに次の弾のライフサイクルを開始（処理落ち時はまとめて追い付く）
        float rainElapsed = elapsedTime_ - chargeTime_;
        while (nextSpawnIndex_ < totalSpawnCount_ &&
               rainElapsed >= static_cast<float>(nextSpawnIndex_) * spawnInterval_) {
            int slot = FindFreeSlot();
            if (slot < 0) {
                // プール枯渇時はこの1発を諦め、硬直開始時刻の事前計算との整合を優先
                nextSpawnIndex_++;
                continue;
            }
            ActivateBullet(slot, blackboard);
            nextSpawnIndex_++;
        }
    }

    for (int i = 0; i < poolSize_; ++i) {
        UpdateBullet(i, boss, deltaTime);
    }

    if (elapsedTime_ >= recoveryStart_) {
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossBulletRain::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    nextSpawnIndex_ = 0;
    fallTime_ = fallHeight_ / (std::max)(fallSpeed_, 0.01f);

    // t=0 に1発 + spawnInterval_ ごとに1発を rainDuration_ の間続ける
    float interval = (std::max)(spawnInterval_, 0.01f);
    totalSpawnCount_ = static_cast<int>(rainDuration_ / interval) + 1;

    // スロットは同時進行の最大数 + 余裕2 だけ確保し、Done を再利用する
    float bulletLifecycle = warningDuration_ + blinkDuration_ + fallTime_ + impactActiveDuration_;
    poolSize_ = std::clamp(static_cast<int>(std::ceil(bulletLifecycle / interval)) + 2, 1, kMaxPoolSize);
    poolSize_ = (std::min)(poolSize_, totalSpawnCount_);

    // 最終弾のライフサイクル終了 = 硬直開始
    recoveryStart_ = chargeTime_ + static_cast<float>(totalSpawnCount_ - 1) * interval + bulletLifecycle;
    totalDuration_ = recoveryStart_ + recoveryTime_;

    bulletSignEffect_.Start(boss, chargeTime_);

    bullets_.assign(poolSize_, Bullet{});

    colliderTransforms_.clear();
    colliderTransforms_.reserve(poolSize_);
    for (int i = 0; i < poolSize_; ++i) {
        Transform t;
        t.translate = Vector3(0.0f, 0.0f, 0.0f);
        t.rotate = Vector3(0.0f, 0.0f, 0.0f);
        t.scale = Vector3(1.0f, 1.0f, 1.0f);
        colliderTransforms_.push_back(t);
    }

    decals_.clear();
    decals_.reserve(poolSize_);
    colliders_.clear();
    colliders_.reserve(poolSize_);

    float diameter = impactRadius_ * 2.0f;

    for (int i = 0; i < poolSize_; ++i) {
        auto decal = std::make_unique<Decal>();
        decal->Initialize();
        decal->SetShape(DecalShape::Circle);
        decal->SetScale(Vector3(diameter, 1.0f, diameter));
        decal->SetEdgeSoftness(0.02f);
        decal->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalBaseAlpha));
        decal->SetVisible(false);
        decals_.push_back(std::move(decal));

        BossAttackHitboxRef collider;
        collider.Spawn(boss, hitboxPrefab_, colliderTransforms_[i]);
        if (collider) {
            collider->SetRadius(impactRadius_);
            collider->SetDamage(damage_);
        }
        colliders_.push_back(collider);
    }
}

void BTBossBulletRain::OnCleanup() {
    // チャージ中に中断された場合も予兆エミッターを残さない
    if (bulletSignEffect_.IsActive() && cachedBoss_) {
        bulletSignEffect_.End(cachedBoss_);
    }

    decals_.clear();

    for (BossAttackHitboxRef& collider : colliders_) {
        collider.Destroy();
    }
    colliders_.clear();

    colliderTransforms_.clear();
    bullets_.clear();
    poolSize_ = 0;
    totalSpawnCount_ = 0;
    nextSpawnIndex_ = 0;
}

int BTBossBulletRain::FindFreeSlot() const {
    for (int i = 0; i < poolSize_; ++i) {
        if (bullets_[i].state == BulletState::Waiting || bullets_[i].state == BulletState::Done) {
            return i;
        }
    }
    return -1;
}

void BTBossBulletRain::ActivateBullet(int index, Tako::BTBlackboard* blackboard) {
    Player* player = blackboard ? blackboard->GetPtr<Player>("player") : nullptr;
    Vector3 pos = SampleImpactPosition(player);

    bullets_[index].state = BulletState::Warning;
    bullets_[index].stateTimer = 0.0f;
    bullets_[index].impactPos = pos;

    colliderTransforms_[index].translate = Vector3(pos.x, colliderY_, pos.z);
    if (colliders_[index]) {
        colliders_[index]->SetTransform(colliderTransforms_[index]);
    }

    if (decals_[index]) {
        decals_[index]->SetTranslate(Vector3(pos.x, 0.0f, pos.z));
        decals_[index]->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalBaseAlpha));
        decals_[index]->SetVisible(true);
    }
}

Tako::Vector3 BTBossBulletRain::SampleImpactPosition(Player* player) {
    RandomEngine* rng = RandomEngine::GetInstance();

    BossMovement::AreaBounds bounds{
        GameConst::StageXMin() + stageMargin_, GameConst::StageXMax() - stageMargin_,
        GameConst::StageZMin() + stageMargin_, GameConst::StageZMax() - stageMargin_
    };

    const bool aimAtPlayer = player && rng->GetNormalized() < playerAimProbability_;
    const float minSepSq = minSeparation_ * minSeparation_;

    Vector3 candidate(0.0f, 0.0f, 0.0f);
    for (int retry = 0; retry < kMaxPlacementRetries; ++retry) {
        if (aimAtPlayer) {
            Vector3 playerPos = player->GetTransform().translate;
            // sqrt で半径方向の偏りを補正した円内一様分布
            float r = playerAimRadius_ * std::sqrt(rng->GetNormalized());
            float angle = rng->GetAngle();
            candidate = BossMovement::ClampToBounds(
                Vector3(playerPos.x + std::cos(angle) * r, 0.0f, playerPos.z + std::sin(angle) * r), bounds);
        }
        else {
            candidate = Vector3(rng->GetFloat(bounds.xMin, bounds.xMax), 0.0f,
                                rng->GetFloat(bounds.zMin, bounds.zMax));
        }

        if (minSeparation_ <= 0.0f) {
            return candidate;
        }

        // 進行中の弾とだけ間隔チェック（Done 済みの場所への再着弾は許容）
        bool tooClose = false;
        for (int i = 0; i < poolSize_; ++i) {
            const Bullet& b = bullets_[i];
            if (b.state == BulletState::Waiting || b.state == BulletState::Done) {
                continue;
            }
            if (candidate.DistanceSquared(b.impactPos) < minSepSq) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            return candidate;
        }
    }

    // リトライ回数制限オーバーした場合は重なりを許容して採用（弾数は減らさない）
    return candidate;
}

void BTBossBulletRain::UpdateBullet(int index, Boss* boss, float deltaTime) {
    Bullet& b = bullets_[index];

    switch (b.state) {
    case BulletState::Waiting:
    case BulletState::Done:
        return;

    case BulletState::Warning:
        if (b.stateTimer >= warningDuration_) {
            b.state = BulletState::Blinking;
            b.stateTimer = 0.0f;
        }
        break;

    case BulletState::Blinking: {
        float sinValue = std::abs(std::sin(b.stateTimer * blinkFrequency_ * 3.14159265f));
        float alpha = kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue;
        if (decals_[index]) {
            decals_[index]->SetColor(Vector4(1.0f, 0.2f, 0.1f, alpha));
        }
        if (b.stateTimer >= blinkDuration_) {
            b.state = BulletState::Falling;
            b.stateTimer = 0.0f;
            if (decals_[index]) {
                decals_[index]->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalFallingAlpha));
            }
            boss->RequestBulletSpawn(Vector3(b.impactPos.x, fallHeight_, b.impactPos.z),
                                     Vector3(0.0f, -fallSpeed_, 0.0f));
        }
        break;
    }

    case BulletState::Falling:
        // 見た目の弾が地面へ到達する瞬間 (fallTime_ 経過) に判定を有効化
        if (b.stateTimer >= fallTime_) {
            b.state = BulletState::Impact;
            b.stateTimer = 0.0f;
            if (decals_[index]) {
                decals_[index]->SetVisible(false);
            }
            if (colliders_[index]) {
                colliders_[index]->SetActive(true);
            }
        }
        break;

    case BulletState::Impact:
        if (b.stateTimer >= impactActiveDuration_) {
            b.state = BulletState::Done;
            if (colliders_[index]) {
                colliders_[index]->SetActive(false);
            }
        }
        break;
    }

    b.stateTimer += deltaTime;
}

#ifdef _DEBUG
bool BTBossBulletRain::DrawCustomImGui() {
    float interval = (std::max)(spawnInterval_, 0.01f);
    int totalBullets = static_cast<int>(rainDuration_ / interval) + 1;
    float lifecycle = warningDuration_ + blinkDuration_ + fallHeight_ / (std::max)(fallSpeed_, 0.01f) + impactActiveDuration_;
    int pool = std::clamp(static_cast<int>(std::ceil(lifecycle / interval)) + 2, 1, kMaxPoolSize);
    ImGui::TextDisabled("Bullets: %d | Pool: %d/%d", totalBullets, (std::min)(pool, totalBullets), kMaxPoolSize);
    return false;
}
#endif
