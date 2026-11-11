#include "BTBossMeteorRain.h"
#include "../../Boss.h"
#include "../../../Player/Player.h"
#include "../../../../Common/GameConst.h"
#include "RandomEngine.h"

#include <cmath>
#include <algorithm>

using namespace Tako;

BTBossMeteorRain::BTBossMeteorRain() {
    name_ = "BossMeteorRain";
}

BTBossMeteorRain::~BTBossMeteorRain() {
    OnCleanup();
}

Tako::BTNodeStatus BTBossMeteorRain::OnExecute(Tako::BTBlackboard* /*blackboard*/, Boss* boss, float deltaTime) {
    // フェーズ管理: Charge → Launch → Warning → Blink → Impact → Recovery
    const float chargeEnd = chargeTime_;
    const float launchEnd = chargeEnd + launchDuration_;
    const float warningEnd = launchEnd + warningDuration_;
    const float blinkEnd = warningEnd + blinkDuration_;
    const float impactEnd = blinkEnd + impactDuration_;

    if (elapsedTime_ < chargeEnd) {
        bulletSignEffect_.Update(boss, deltaTime);
    }
    else if (elapsedTime_ < launchEnd) {
        if (bulletSignEffect_.IsActive()) {
            bulletSignEffect_.End(boss);
        }

        float launchElapsed = elapsedTime_ - chargeEnd;

        // 上方向弾を launchDuration_ にわたり等間隔で発射
        float launchInterval = (impactCount_ > 0) ? launchDuration_ / static_cast<float>(impactCount_) : launchDuration_;
        int expectedLaunched = static_cast<int>(launchElapsed / launchInterval);
        expectedLaunched = std::min<int>(expectedLaunched, impactCount_);

        while (bulletsLaunched_ < expectedLaunched) {
            LaunchBullet(boss, bulletsLaunched_);
            bulletsLaunched_++;
        }

        // 水平弾（上方向弾とは独立タイミング）
        if (horizontalBulletCount_ > 0) {
            Vector3 bossPos = boss->GetTransform().translate;
            float hInterval = launchDuration_ / static_cast<float>(horizontalBulletCount_);
            int expectedH = std::min<int>(static_cast<int>(launchElapsed / hInterval), horizontalBulletCount_);
            while (horizontalBulletsLaunched_ < expectedH) {
                float angle = (2.0f * 3.14159265f / static_cast<float>(horizontalBulletCount_)) * static_cast<float>(horizontalBulletsLaunched_);
                float vx = std::cos(angle) * horizontalSpeed_;
                float vz = std::sin(angle) * horizontalSpeed_;
                boss->RequestBulletSpawn(bossPos, Vector3(vx, 0.0f, vz));
                horizontalBulletsLaunched_++;
            }
        }
    }
    else if (elapsedTime_ < warningEnd) {
        // 取りこぼした上方向弾を発射
        while (bulletsLaunched_ < impactCount_) {
            LaunchBullet(boss, bulletsLaunched_);
            bulletsLaunched_++;
        }
        // 取りこぼした水平弾を発射
        if (horizontalBulletCount_ > 0) {
            Vector3 bossPos = boss->GetTransform().translate;
            while (horizontalBulletsLaunched_ < horizontalBulletCount_) {
                float angle = (2.0f * 3.14159265f / static_cast<float>(horizontalBulletCount_)) * static_cast<float>(horizontalBulletsLaunched_);
                float vx = std::cos(angle) * horizontalSpeed_;
                float vz = std::sin(angle) * horizontalSpeed_;
                boss->RequestBulletSpawn(bossPos, Vector3(vx, 0.0f, vz));
                horizontalBulletsLaunched_++;
            }
        }
        // Decal 表示は1回のみ
        if (!decalsShown_) {
            for (int i = 0; i < impactCount_; ++i) {
                if (impactDecals_[i]) {
                    impactDecals_[i]->SetVisible(true);
                }
            }
            decalsShown_ = true;
        }
    }
    else if (elapsedTime_ < blinkEnd) {
        UpdateBlinkingPhase(elapsedTime_ - warningEnd);
    }
    else if (elapsedTime_ < impactEnd) {
        if (!hasBegunImpact_) {
            BeginImpactPhase(boss);
            hasBegunImpact_ = true;
        }
    }
    else {
        if (!hasEndedImpact_) {
            EndImpactPhase(boss);
            hasEndedImpact_ = true;
        }
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        hasBegunImpact_ = false;
        hasEndedImpact_ = false;
        decalsShown_ = false;
        bulletsLaunched_ = 0;
        horizontalBulletsLaunched_ = 0;
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossMeteorRain::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    hasBegunImpact_ = false;
    hasEndedImpact_ = false;
    decalsShown_ = false;
    bulletsLaunched_ = 0;
    horizontalBulletsLaunched_ = 0;

    totalDuration_ = chargeTime_ + launchDuration_ + warningDuration_ + blinkDuration_ + impactDuration_ + recoveryTime_;

    bulletSignEffect_.Start(boss, chargeTime_);

    RandomEngine* rng = RandomEngine::GetInstance();
    impactCount_ = rng->GetInt(minImpacts_, maxImpacts_);
    impactCount_ = std::clamp(impactCount_, 1, kMaxImpacts);

    Vector3 bossPos = boss->GetTransform().translate;
    GenerateImpactPositions(bossPos);

    colliderTransforms_.clear();
    colliderTransforms_.reserve(impactCount_);
    for (int i = 0; i < impactCount_; ++i) {
        Transform t;
        t.translate = impactPositions_[i];
        t.rotate = Vector3(0.0f, 0.0f, 0.0f);
        t.scale = Vector3(1.0f, 1.0f, 1.0f);
        colliderTransforms_.push_back(t);
    }

    impactDecals_.clear();
    impactDecals_.reserve(impactCount_);
    impactColliders_.clear();
    impactColliders_.reserve(impactCount_);

    float diameter = impactRadius_ * 2.0f;

    for (int i = 0; i < impactCount_; ++i) {
        auto decal = std::make_unique<Decal>();
        decal->Initialize();
        decal->SetShape(DecalShape::Circle);
        decal->SetTranslate(Vector3(impactPositions_[i].x, 0.0f, impactPositions_[i].z));
        decal->SetScale(Vector3(diameter, 1.0f, diameter));
        decal->SetEdgeSoftness(0.02f);
        decal->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalBaseAlpha));
        decal->SetVisible(false);
        impactDecals_.push_back(std::move(decal));

        BossAttackHitboxRef collider;
        collider.Spawn(boss, hitboxPrefab_, colliderTransforms_[i]);
        if (collider) {
            collider->SetRadius(impactRadius_);
            collider->SetDamage(damage_);
        }
        impactColliders_.push_back(collider);
    }
}

void BTBossMeteorRain::OnCleanup() {
    // チャージ中に中断された場合も予兆エミッターを残さない
    if (bulletSignEffect_.IsActive() && cachedBoss_) {
        bulletSignEffect_.End(cachedBoss_);
    }

    impactDecals_.clear();

    for (BossAttackHitboxRef& collider : impactColliders_) {
        collider.Destroy();
    }
    impactColliders_.clear();

    colliderTransforms_.clear();
    impactPositions_.clear();
    impactCount_ = 0;
    hasBegunImpact_ = false;
    hasEndedImpact_ = false;
    decalsShown_ = false;
    bulletsLaunched_ = 0;
    horizontalBulletsLaunched_ = 0;
}

void BTBossMeteorRain::GenerateImpactPositions(const Vector3& bossPos) {
    impactPositions_.clear();
    impactPositions_.reserve(impactCount_);

    RandomEngine* rng = RandomEngine::GetInstance();
    float areaSize = GameConst::BossPhase2AreaSize();

    float minSeparation = 2.0f * impactRadius_;
    float minSepSq = minSeparation * minSeparation;

    for (int i = 0; i < impactCount_; ++i) {
        bool placed = false;

        for (int retry = 0; retry < kMaxPlacementRetries; ++retry) {
            float x = bossPos.x + rng->GetFloat(-areaSize, areaSize);
            float z = bossPos.z + rng->GetFloat(-areaSize, areaSize);
            Vector3 candidate(x, bossPos.y, z);

            bool tooClose = false;
            for (const auto& existing : impactPositions_) {
                if (candidate.DistanceSquared(existing) < minSepSq) {
                    tooClose = true;
                    break;
                }
            }

            if (!tooClose) {
                impactPositions_.push_back(candidate);
                placed = true;
                break;
            }
        }

        if (!placed) {
            impactCount_ = static_cast<int>(impactPositions_.size());
            break;
        }
    }
}

void BTBossMeteorRain::LaunchBullet(Boss* boss, int /*index*/) {
    Vector3 bossPos = boss->GetTransform().translate;
    RandomEngine* rng = RandomEngine::GetInstance();

    float spreadX = rng->GetFloat(-launchSpreadXZ_, launchSpreadXZ_);
    float spreadZ = rng->GetFloat(-launchSpreadXZ_, launchSpreadXZ_);
    boss->RequestBulletSpawn(bossPos, Vector3(spreadX, launchSpeed_, spreadZ));
}

void BTBossMeteorRain::UpdateBlinkingPhase(float phaseElapsed) {
    float sinValue = std::abs(std::sin(phaseElapsed * blinkFrequency_ * 3.14159265f));
    float alpha = kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue;

    for (int i = 0; i < impactCount_; ++i) {
        if (impactDecals_[i]) {
            impactDecals_[i]->SetColor(Vector4(1.0f, 0.2f, 0.1f, alpha));
        }
    }
}

void BTBossMeteorRain::BeginImpactPhase(Boss* boss) {
    for (int i = 0; i < impactCount_; ++i) {
        if (impactDecals_[i]) impactDecals_[i]->SetVisible(false);
    }
    for (int i = 0; i < impactCount_; ++i) {
        if (impactColliders_[i]) impactColliders_[i]->SetActive(true);
    }

    // 各着弾位置の上空から弾を落下スポーン
    for (int i = 0; i < impactCount_; ++i) {
        Vector3 spawnPos(impactPositions_[i].x, impactPositions_[i].y + fallHeight_, impactPositions_[i].z);
        boss->RequestBulletSpawn(spawnPos, Vector3(0.0f, -fallSpeed_, 0.0f));
    }
}

void BTBossMeteorRain::EndImpactPhase(Boss* /*boss*/) {
    for (int i = 0; i < impactCount_; ++i) {
        if (impactDecals_[i])    impactDecals_[i]->SetVisible(false);
        if (impactColliders_[i]) impactColliders_[i]->SetActive(false);
    }
}

void BTBossMeteorRain::PostEditChange() {
    maxImpacts_ = (std::max)(maxImpacts_, minImpacts_);
}
