#include "BTBossAreaAttack.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include "Source/Common/GameConst.h"

#include "RandomEngine.h"
#include "EmitterManager.h"

#include <cmath>
#include <algorithm>

using namespace Tako;

BTBossAreaAttack::BTBossAreaAttack() {
    name_ = "BossAreaAttack";
}

BTBossAreaAttack::~BTBossAreaAttack() {
    OnCleanup();
}

Tako::BTNodeStatus BTBossAreaAttack::OnExecute(Tako::BTBlackboard* /*blackboard*/, Boss* boss, float deltaTime) {
    // フェーズ管理: Warning → Blinking → Attack → Recovery
    const float warningEnd = warningDuration_;
    const float blinkEnd = warningEnd + blinkDuration_;
    const float attackEnd = blinkEnd + attackDuration_;

    if (elapsedTime_ < warningEnd) {

    }
    else if (elapsedTime_ < blinkEnd) {
        UpdateBlinkingPhase(elapsedTime_ - warningEnd);
    }
    else if (elapsedTime_ < attackEnd) {
        if (!hasBegunAttack_) {
            BeginAttackPhase(boss);
            hasBegunAttack_ = true;
        }
    }
    else {
        if (!hasEndedAttack_) {
            EndAttackPhase(boss);
            hasEndedAttack_ = true;
        }
        EnterAttackRecovery(boss);
    }

    elapsedTime_ += deltaTime;

    if (elapsedTime_ >= totalDuration_) {
        hasBegunAttack_ = false;
        hasEndedAttack_ = false;
        return FinishAttack();
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossAreaAttack::OnInitialize(Tako::BTBlackboard* blackboard, Boss* boss) {
    hasBegunAttack_ = false;
    hasEndedAttack_ = false;

    totalDuration_ = warningDuration_ + blinkDuration_ + attackDuration_ + recoveryTime_;

    SelectRandomQuadrants(blackboard);

    Vector3 bossPos = boss->GetTransform().translate;
    float halfArea = GameConst::BossPhase2AreaSize() * 0.5f;

    for (int i = 0; i < kQuadrantCount; ++i) {
        Vector3 center = GetQuadrantCenter(i, bossPos);

        quadrantDecals_[i] = std::make_unique<Decal>();
        quadrantDecals_[i]->Initialize();
        quadrantDecals_[i]->SetShape(DecalShape::Rectangle);
        quadrantDecals_[i]->SetTranslate(Vector3(center.x, 0.0f, center.z));
        quadrantDecals_[i]->SetScale(Vector3(halfArea * 2.0f, 1.0f, halfArea * 2.0f));
        quadrantDecals_[i]->SetEdgeSoftness(0.02f);

        if (activeQuadrants_[i]) {
            quadrantDecals_[i]->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalBaseAlpha));
            quadrantDecals_[i]->SetVisible(true);
        } else {
            quadrantDecals_[i]->SetVisible(false);
        }

        colliderTransforms_[i].translate = center;
        colliderTransforms_[i].rotate = Vector3(0.0f, 0.0f, 0.0f);
        colliderTransforms_[i].scale = Vector3(1.0f, 1.0f, 1.0f);

        quadrantColliders_[i].Spawn(boss, hitboxPrefab_, colliderTransforms_[i]);
        if (quadrantColliders_[i]) {
            quadrantColliders_[i]->SetSize(Vector3(halfArea * 2.0f, kColliderHeight, halfArea * 2.0f));
            quadrantColliders_[i]->SetDamage(damage_);
        }
    }

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && !particlesInitialized_) {
        for (int i = 0; i < kQuadrantCount; ++i) {
            emitterNames_[i] = "area_attack_q" + std::to_string(i);
            emitterMgr->LoadPreset("attack_slash", emitterNames_[i]);
            emitterMgr->SetEmitterActive(emitterNames_[i], false);
        }
        particlesInitialized_ = true;
    }
}

void BTBossAreaAttack::OnCleanup() {
    // 攻撃中に中断された場合のみエミッタを停止する
    if (cachedEmitterManager_ && particlesInitialized_ && hasBegunAttack_ && !hasEndedAttack_) {
        for (int i = 0; i < kQuadrantCount; ++i) {
            if (activeQuadrants_[i]) {
                cachedEmitterManager_->SetEmitterActive(emitterNames_[i], false);
            }
        }
    }

    for (int i = 0; i < kQuadrantCount; ++i) {
        quadrantDecals_[i].reset();
    }

    for (int i = 0; i < kQuadrantCount; ++i) {
        quadrantColliders_[i].Destroy();
    }

    activeQuadrants_.fill(false);
    hasBegunAttack_ = false;
    hasEndedAttack_ = false;
}

void BTBossAreaAttack::SelectRandomQuadrants(Tako::BTBlackboard* blackboard) {
    RandomEngine* rng = RandomEngine::GetInstance();

    int count = rng->GetInt(minQuadrants_, maxQuadrants_);
    count = std::clamp(count, 1, kQuadrantCount);

    activeQuadrants_.fill(false);

    int playerQuadrant = GetPlayerQuadrant(blackboard);
    activeQuadrants_[playerQuadrant] = true;

    if (count > 1) {
        std::array<int, kQuadrantCount - 1> remaining;
        int idx = 0;
        for (int i = 0; i < kQuadrantCount; ++i) {
            if (i != playerQuadrant) {
                remaining[idx++] = i;
            }
        }

        // Fisher-Yates シャッフル
        for (int i = static_cast<int>(remaining.size()) - 1; i > 0; --i) {
            int j = rng->GetInt(0, i);
            std::swap(remaining[i], remaining[j]);
        }

        for (int i = 0; i < count - 1; ++i) {
            activeQuadrants_[remaining[i]] = true;
        }
    }
}

int BTBossAreaAttack::GetPlayerQuadrant(Tako::BTBlackboard* blackboard) const {
    Boss* boss = blackboard->GetPtr<Boss>("boss");
    Player* player = blackboard->GetPtr<Player>("player");
    Vector3 bossPos = boss->GetTransform().translate;
    Vector3 playerPos = player->GetTransform().translate;

    int xIndex = (playerPos.x >= bossPos.x) ? 1 : 0;
    int zIndex = (playerPos.z >= bossPos.z) ? 1 : 0;
    return zIndex * 2 + xIndex;
}

Vector3 BTBossAreaAttack::GetQuadrantCenter(int quadrantIndex, const Vector3& bossPos) const {
    float halfArea = GameConst::BossPhase2AreaSize() * 0.5f;

    float xSign = (quadrantIndex % 2 == 0) ? -1.0f : 1.0f;
    float zSign = (quadrantIndex < 2) ? -1.0f : 1.0f;

    return Vector3(bossPos.x + xSign * halfArea, bossPos.y, bossPos.z + zSign * halfArea);
}

void BTBossAreaAttack::UpdateBlinkingPhase(float phaseElapsed) {
    float sinValue = std::abs(std::sin(phaseElapsed * blinkFrequency_ * std::numbers::pi_v<float>));
    float alpha = kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue;

    for (int i = 0; i < kQuadrantCount; ++i) {
        if (activeQuadrants_[i] && quadrantDecals_[i]) {
            quadrantDecals_[i]->SetColor(Vector4(1.0f, 0.2f, 0.1f, alpha));
        }
    }
}

void BTBossAreaAttack::BeginAttackPhase(Boss* boss) {
    for (int i = 0; i < kQuadrantCount; ++i) {
        if (activeQuadrants_[i] && quadrantDecals_[i]) {
            quadrantDecals_[i]->SetVisible(false);
        }
    }
    for (int i = 0; i < kQuadrantCount; ++i) {
        if (activeQuadrants_[i] && quadrantColliders_[i]) {
            quadrantColliders_[i]->SetActive(true);
        }
    }

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && particlesInitialized_) {
        Vector3 bossPos = boss->GetTransform().translate;
        for (int i = 0; i < kQuadrantCount; ++i) {
            if (activeQuadrants_[i]) {
                Vector3 center = GetQuadrantCenter(i, bossPos);
                emitterMgr->SetEmitterPosition(emitterNames_[i], center);
                emitterMgr->SetEmitterActive(emitterNames_[i], true);
            }
        }
    }
}

void BTBossAreaAttack::EndAttackPhase(Boss* boss) {
    for (int i = 0; i < kQuadrantCount; ++i) {
        if (quadrantDecals_[i]) quadrantDecals_[i]->SetVisible(false);
    }
    for (int i = 0; i < kQuadrantCount; ++i) {
        if (quadrantColliders_[i]) quadrantColliders_[i]->SetActive(false);
    }

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && particlesInitialized_) {
        for (int i = 0; i < kQuadrantCount; ++i) {
            emitterMgr->SetEmitterActive(emitterNames_[i], false);
        }
    }
}

void BTBossAreaAttack::PostEditChange() {
    maxQuadrants_ = (std::max)(maxQuadrants_, minQuadrants_);
}
