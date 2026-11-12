#include "BTBossSlashAttack.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"

#include "EmitterManager.h"

#include <cmath>

using namespace Tako;

BTBossSlashAttack::BTBossSlashAttack() {
    name_ = "BossSlashAttack";
}

BTBossSlashAttack::~BTBossSlashAttack() {
    OnCleanup();
}

Tako::BTNodeStatus BTBossSlashAttack::OnExecute(Tako::BTBlackboard* /*blackboard*/, Boss* boss, float deltaTime) {
    const float warningEnd = warningDuration_;
    const float blinkEnd = warningEnd + blinkDuration_;
    const float attackEnd = blinkEnd + attackDuration_;

    if (elapsedTime_ < warningEnd) {
        // Warning: Decal を固定アルファで表示（OnInitialize で設定済み）
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
        // 多重呼び出しは Helper がガード
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

void BTBossSlashAttack::OnInitialize(Tako::BTBlackboard* blackboard, Boss* boss) {
    hasBegunAttack_ = false;
    hasEndedAttack_ = false;

    totalDuration_ = warningDuration_ + blinkDuration_ + attackDuration_ + recoveryTime_;

    // 発動時のプレイヤー位置を着弾点として固定
    Vector3 targetPos = blackboard->GetPtr<Player>("player")->GetTransform().translate;

    float diameter = attackRadius_ * 2.0f;
    slashDecal_ = std::make_unique<Decal>();
    slashDecal_->Initialize();
    slashDecal_->SetShape(DecalShape::Circle);
    slashDecal_->SetTranslate(Vector3(targetPos.x, 0.0f, targetPos.z));
    slashDecal_->SetScale(Vector3(diameter, 1.0f, diameter));
    slashDecal_->SetEdgeSoftness(0.02f);
    slashDecal_->SetColor(Vector4(1.0f, 0.2f, 0.1f, kDecalBaseAlpha));
    slashDecal_->SetVisible(true);

    colliderTransform_.translate = targetPos;
    colliderTransform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    colliderTransform_.scale = Vector3(1.0f, 1.0f, 1.0f);

    slashCollider_.Spawn(boss, hitboxPrefab_, colliderTransform_);
    if (slashCollider_) {
        slashCollider_->SetRadius(attackRadius_);
        slashCollider_->SetDamage(damage_);
    }

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && !particleInitialized_) {
        emitterName_ = "slash_attack_0";
        emitterMgr->LoadPreset("sphere_attack_slash", emitterName_);
        emitterMgr->SetEmitterActive(emitterName_, false);
        particleInitialized_ = true;
    }
}

void BTBossSlashAttack::UpdateBlinkingPhase(float phaseElapsed) {
    float sinValue = std::abs(std::sin(phaseElapsed * blinkFrequency_ * 3.14159265f));
    float alpha = kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue;
    if (slashDecal_) {
        slashDecal_->SetColor(Vector4(1.0f, 0.2f, 0.1f, alpha));
    }
}

void BTBossSlashAttack::BeginAttackPhase(Boss* boss) {
    if (slashDecal_)    slashDecal_->SetVisible(false);
    if (slashCollider_) slashCollider_->SetActive(true);

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && particleInitialized_) {
        emitterMgr->SetEmitterPosition(emitterName_, colliderTransform_.translate);
        emitterMgr->SetEmitterRadius(emitterName_, attackRadius_);
        emitterMgr->SetEmitterActive(emitterName_, true);
    }
}

void BTBossSlashAttack::EndAttackPhase(Boss* boss) {
    if (slashCollider_) slashCollider_->SetActive(false);

    EmitterManager* emitterMgr = boss->GetEmitterManager();
    if (emitterMgr && particleInitialized_) {
        emitterMgr->SetEmitterActive(emitterName_, false);
    }
}

void BTBossSlashAttack::OnCleanup() {
    // 攻撃フェーズに入ったまま中断された場合のみエミッタを停止する
    // （Warning/Blinking では未起動、EndAttackPhase 後は停止済み）
    if (cachedEmitterManager_ && particleInitialized_ && hasBegunAttack_ && !hasEndedAttack_) {
        cachedEmitterManager_->SetEmitterActive(emitterName_, false);
    }

    slashDecal_.reset();

    slashCollider_.Destroy();

    hasBegunAttack_ = false;
    hasEndedAttack_ = false;
}

