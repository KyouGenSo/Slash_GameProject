#include "BTBossVortexTempest.h"
#include "Script/Object/Boss/Boss.h"
#include "Script/Object/Player/Player.h"
#include "EmitterManager.h"
#include "ForceFieldManager.h"
#include "ParticleStruct.h"
#include <algorithm>
#include <cmath>

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif

using namespace Tako;

namespace {
    constexpr float kPi = 3.14159265358979323846f;

    /// <summary>
    /// v 番目の渦点の基準角度（0, π/2, π, 3π/2）
    /// </summary>
    inline float BaseAngleFor(int v) {
        return static_cast<float>(v) * (kPi * 0.5f);
    }

    inline const std::string& PresetNameForSlot(
        int slot,
        const std::string& attractName,
        const std::string& dirName,
        const std::string& vortexName)
    {
        switch (slot) {
        case BTBossVortexTempest::kSlotAttract:     return attractName;
        case BTBossVortexTempest::kSlotDirectional: return dirName;
        case BTBossVortexTempest::kSlotVortex:      return vortexName;
        default:                                    return attractName;
        }
    }
}

BTBossVortexTempest::BTBossVortexTempest() {
    name_ = "BossVortexTempest";
}

Tako::BTNodeStatus BTBossVortexTempest::OnExecute(Tako::BTBlackboard* blackboard, Boss* boss, float deltaTime) {
    ForceFieldManager* ffm = boss->GetForceFieldManager();
    if (!ffm) {
        return Tako::BTNodeStatus::Failure;
    }

    Player* player = blackboard->GetPtr<Player>("player");
    elapsedTime_ += deltaTime;

    const float warningEnd = warningTime_;
    const float expandEnd = warningEnd + expandTime_;
    const float sustainEnd = expandEnd + sustainTime_;
    const float decayEnd = sustainEnd + decayTime_;

    // フェーズ別の強度係数（0〜1）
    float strengthMul = 0.0f;
    bool isFinished = false;

    if (elapsedTime_ < warningEnd) {
        strengthMul = 0.0f;
    }
    else if (elapsedTime_ < expandEnd) {
        const float t = (elapsedTime_ - warningEnd) / std::max<float>(expandTime_, 0.0001f);
        strengthMul = std::clamp(t, 0.0f, 1.0f);
    }
    else if (elapsedTime_ < sustainEnd) {
        strengthMul = 1.0f;
    }
    else if (elapsedTime_ < decayEnd) {
        const float t = (elapsedTime_ - sustainEnd) / std::max<float>(decayTime_, 0.0001f);
        strengthMul = 1.0f - std::clamp(t, 0.0f, 1.0f);
        // 終息フェーズで硬直に入る（プレイヤーがスタンを取れる隙）
        EnterAttackRecovery(boss);
    }
    else {
        isFinished = true;
    }

    if (isFinished) {
        // 累積した未適用 DoT を一括 flush。
        // Reset（中断）経路では flush せず、攻撃終了後の不自然な遅延ダメージを防ぐ。
        if (player && pendingDamage_ > 0.0f) {
            player->OnHit(pendingDamage_);
            pendingDamage_ = 0.0f;
        }
        return FinishAttack();
    }

    const Vector3 bossPos = boss->GetTransform().translate;
    Vector3 vortexPositions[kVortexCount];

    float decalAlpha = 0.0f;
    if (elapsedTime_ < warningEnd) {
        // Phase 0 予兆：sin 点滅
        const float sinValue = std::abs(std::sin(elapsedTime_ * markerBlinkFrequency_ * kPi));
        decalAlpha = kBlinkAlphaMin + kBlinkAlphaAmplitude * sinValue;
    }
    else if (elapsedTime_ < sustainEnd) {
        decalAlpha = kDecalBaseAlpha;
    }
    else {
        // 終息：線形フェードアウト
        const float t = (elapsedTime_ - sustainEnd) / std::max<float>(decayTime_, 0.0001f);
        decalAlpha = kDecalBaseAlpha * (1.0f - std::clamp(t, 0.0f, 1.0f));
    }

    for (int v = 0; v < kVortexCount; ++v) {
        const float baseAngle = BaseAngleFor(v);
        const float currentAngle = baseAngle + angularSpeed_ * elapsedTime_;
        const Vector3 offset = { std::cos(currentAngle), 0.0f, std::sin(currentAngle) };
        const Vector3 vortexPos = bossPos + offset * orbitRadius_;
        vortexPositions[v] = vortexPos;

        for (int s = 0; s < kFieldsPerVortex; ++s) {
            if (loadedFields_[v][s].index < 0) continue;

            ForceFieldData updated = loadedFields_[v][s].base;
            updated.position.x = vortexPos.x;
            updated.position.z = vortexPos.z;
            updated.position.y = bossPos.y + loadedFields_[v][s].base.position.y;
            updated.strength = loadedFields_[v][s].base.strength * strengthMul;

            ffm->UpdateForceField(static_cast<uint32_t>(loadedFields_[v][s].index), updated);
        }

        if (cachedEmitterManager_) {
            cachedEmitterManager_->SetEmitterPosition(MakeVortexEmitterName(v), vortexPos);
        }

        // 地面マーカーデカール: Y=0 固定で地面に貼り付け
        if (vortexDecals_[v]) {
            vortexDecals_[v]->SetTranslate(Vector3(vortexPos.x, 0.0f, vortexPos.z));
            vortexDecals_[v]->SetColor(Vector4(1.0f, 0.2f, 0.1f, decalAlpha));
        }
    }

    // DoT 判定（展開完了後に適用、累積バッファ + Tick 方式）
    if (player && elapsedTime_ >= expandEnd) {
        const Vector3 playerPos = player->GetTransform().translate;
        const float dotRadius = loadedFields_[0][kSlotAttract].base.radius;

        if (dotRadius > 0.0f) {
            for (int v = 0; v < kVortexCount; ++v) {
                const Vector3 attractCenter = {
                    vortexPositions[v].x,
                    bossPos.y + loadedFields_[v][kSlotAttract].base.position.y,
                    vortexPositions[v].z,
                };
                const float dist = (playerPos - attractCenter).Length();
                if (dist < dotRadius) {
                    const float t = std::clamp(dist / dotRadius, 0.0f, 1.0f);
                    const float dotPerSec = maxDoT_ * (1.0f - t) + minDoT_ * t;
                    pendingDamage_ += dotPerSec * deltaTime;
                    break;  // 二重ダメージ防止
                }
            }
        }

        damageTickTimer_ += deltaTime;
        if (damageTickTimer_ >= damageTickInterval_ && pendingDamage_ > 0.0f) {
            player->OnHit(pendingDamage_);
            pendingDamage_ = 0.0f;
            damageTickTimer_ = 0.0f;
        }
    }

    return Tako::BTNodeStatus::Running;
}

void BTBossVortexTempest::OnInitialize(Tako::BTBlackboard* /*blackboard*/, Boss* boss) {
    ForceFieldManager* ffm = boss->GetForceFieldManager();
    if (!ffm) return;

    cachedForceFieldManager_ = ffm;

    const Vector3 bossPos = boss->GetTransform().translate;

    // 4 渦点を 90 度間隔の円周上に配置
    for (int v = 0; v < kVortexCount; ++v) {
        const float baseAngle = BaseAngleFor(v);
        const Vector3 offset = { std::cos(baseAngle), 0.0f, std::sin(baseAngle) };
        const Vector3 vortexPos = bossPos + offset * orbitRadius_;

        // 各渦点で 3 プリセットを重ねて配置
        for (int s = 0; s < kFieldsPerVortex; ++s) {
            const std::string& presetName = PresetNameForSlot(
                s, attractPresetName_, dirPresetName_, vortexPresetName_);

            if (!ffm->LoadPresetToData(presetName, loadedFields_[v][s].base)) {
                // 読み込み失敗：登録済み分のクリーンアップを基底 Reset に任せるため、ここでは登録だけ続ける
                continue;
            }

            ForceFieldData initial = loadedFields_[v][s].base;
            initial.position.x = vortexPos.x;
            initial.position.z = vortexPos.z;
            initial.position.y = bossPos.y + loadedFields_[v][s].base.position.y;
            initial.strength = 0.0f;
            loadedFields_[v][s].index = ffm->AddForceField(initial);
        }

        // 渦点ごとに 1 個ずつエミッター起動（boss_vortex_0..3）
        if (cachedEmitterManager_) {
            const std::string emitterName = MakeVortexEmitterName(v);
            cachedEmitterManager_->SetEmitterActive(emitterName, true);
            cachedEmitterManager_->SetEmitterPosition(emitterName, vortexPos);
        }

        // 地面マーカーデカール生成（直径 = Attract preset radius * 2）
        const float attractRadius = loadedFields_[v][kSlotAttract].base.radius;
        const float decalDiameter = attractRadius * 2.0f;
        vortexDecals_[v] = std::make_unique<Decal>();
        vortexDecals_[v]->Initialize();
        vortexDecals_[v]->SetShape(DecalShape::Circle);
        vortexDecals_[v]->SetTranslate(Vector3(vortexPos.x, 0.0f, vortexPos.z));
        vortexDecals_[v]->SetScale(Vector3(decalDiameter, 1.0f, decalDiameter));
        vortexDecals_[v]->SetEdgeSoftness(0.02f);
        vortexDecals_[v]->SetColor(Vector4(1.0f, 0.2f, 0.1f, kBlinkAlphaMin));
        vortexDecals_[v]->SetVisible(true);
    }
}

void BTBossVortexTempest::OnCleanup() {
    // ForceField 12 個の削除：erase ベースのため必ず逆順インデックスから削除する。
    if (cachedForceFieldManager_) {
        for (int v = kVortexCount - 1; v >= 0; --v) {
            for (int s = kFieldsPerVortex - 1; s >= 0; --s) {
                if (loadedFields_[v][s].index >= 0) {
                    cachedForceFieldManager_->RemoveForceField(static_cast<uint32_t>(loadedFields_[v][s].index));
                    loadedFields_[v][s].index = -1;
                }
            }
        }
    } else {
        for (int v = 0; v < kVortexCount; ++v) {
            for (int s = 0; s < kFieldsPerVortex; ++s) {
                loadedFields_[v][s].index = -1;
            }
        }
    }
    cachedForceFieldManager_ = nullptr;

    // エミッター停止（4 渦）
    if (cachedEmitterManager_) {
        for (int v = 0; v < kVortexCount; ++v) {
            cachedEmitterManager_->SetEmitterActive(MakeVortexEmitterName(v), false);
        }
    }

    // 地面マーカーデカール解放
    for (auto& decal : vortexDecals_) {
        decal.reset();
    }

    // DoT 累積バッファとタイマーもリセット（適用前残り累積を切り捨て）
    pendingDamage_ = 0.0f;
    damageTickTimer_ = 0.0f;
}

std::string BTBossVortexTempest::MakeVortexEmitterName(int i) const {
    return vortexEmitterBaseName_ + "_" + std::to_string(i);
}

#ifdef _DEBUG
bool BTBossVortexTempest::DrawCustomImGui() {
    ImGui::TextDisabled("Total ForceField count: %d (= %d vortex x %d slots)",
        kVortexCount * kFieldsPerVortex, kVortexCount, kFieldsPerVortex);
    return false;
}
#endif
