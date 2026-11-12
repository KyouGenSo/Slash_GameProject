#include "ShootState.h"
#include "PlayerStateMachine.h"
#include "Script/Object/Player/Player.h"
#include "Script/Object/Boss/Boss.h"
#include "Source/Input/InputHandler.h"
#include "Camera.h"
#include "Matrix4x4.h"
#include "Mat4x4Func.h"
#include "Vec3Func.h"
#include "FrameTimer.h"
#include "Source/Settings/PlayerSettings.h"
#include <algorithm>
#include <cmath>
#include <numbers>

#ifdef _DEBUG
#include "EditorUI.h"
#endif

using namespace Tako;

void ShootState::Enter(Player* player)
{
	// TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Shoot");

	fireRateTimer_ = 0.0f;
}

void ShootState::Update(Player* player, float deltaTime)
{
	const PlayerSettings::Shoot& shoot = Tako::GameSettings::Get<PlayerSettings>().shoot;
	fireRate_ = shoot.fireRate;
	moveSpeedMultiplier_ = shoot.moveSpeedMultiplier;
	aimAssistEnabled_ = shoot.aimAssistEnabled;
	aimAssistAngle_ = shoot.aimAssistAngle;
	aimAssistStrength_ = shoot.aimAssistStrength;

	// 射撃無効時（フェーズ2など）は撃たずに離脱
	if (!player->IsShootingEnabled()) {
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		InputHandler* input = player->GetInputHandler();
		if (stateMachine && input) {
			if (input->IsMoving()) {
				stateMachine->ChangeState("Move");
			} else {
				stateMachine->ChangeState("Idle");
			}
		}
		return;
	}

	if (fireRateTimer_ > 0.0f)
	{
		fireRateTimer_ -= deltaTime;
	}

	CalculateAimDirection(player, deltaTime);

	if (fireRateTimer_ <= 0.0f)
	{
		Fire(player);
		fireRateTimer_ = fireRate_;
	}

    player->Move(deltaTime, moveSpeedMultiplier_, false);
}

void ShootState::Exit(Player* player)
{
	fireRateTimer_ = 0.0f;
}

void ShootState::HandleInput(Player* player)
{
	InputHandler* input = player->GetInputHandler();
	if (!input) return;

	// 射撃ボタンが離されたら元の状態に戻る
	if (!input->IsShooting())
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			if (input->IsMoving())
			{
				stateMachine->ChangeState("Move");
			}
			else
			{
				stateMachine->ChangeState("Idle");
			}
		}
	}
}

void ShootState::CalculateAimDirection(Player* player, float deltaTime)
{
	InputHandler* input = player->GetInputHandler();
	if (!input || !input->IsShooting()) {
		// スティック入力なし → プレイヤー前方
		float yaw = player->GetRotate().y;
		aimDirection_ = Vector3(std::sin(yaw), 0.0f, std::cos(yaw));
		ApplyAimAssist(player);
		return;
	}

	Vector2 stick = input->GetAimDirection();

	// スティック入力を3D 化（X=左右, Y=前後 → X=X, Z=Y）
	Vector3 localDirection = Vector3(stick.x, 0.0f, stick.y);

	// カメラ相対座標系（カメラの Y 回転基準）
	Camera* camera = player->GetCamera();
	float cameraYaw = camera ? camera->GetRotateY() : player->GetRotate().y;
	Matrix4x4 rotationMatrix = Mat4x4::MakeRotateY(cameraYaw);

	aimDirection_ = Mat4x4::TransformNormal(rotationMatrix, localDirection);

    aimDirection_ = aimDirection_.Normalize();

    ApplyAimAssist(player);

    // 発射方向にプレイヤーを向ける
    if (aimDirection_.Length() > 0.01f) {
        float targetAngle = std::atan2(aimDirection_.x, aimDirection_.z);
        float aimRotationLerp = Tako::GameSettings::Get<PlayerSettings>().shoot.aimRotationLerp;
        if (aimRotationLerp <= 0.0f) {
            aimRotationLerp = 0.3f;  // デフォルト値
        }
        Transform* transform = player->GetTransformPtr();
        transform->rotate.y = Vec3::LerpShortAngle(transform->rotate.y, targetAngle, FrameRate::LerpFactor(aimRotationLerp, deltaTime));
    }
}

void ShootState::ApplyAimAssist(Player* player)
{
	if (!aimAssistEnabled_ || aimAssistStrength_ <= 0.0f) return;

	Boss* boss = player->GetTargetEnemy();
	if (!boss || boss->IsDead()) return;

	// XZ平面上の敵方向
	Vector3 toEnemy = boss->GetTranslate() - player->GetTranslate();
	toEnemy.y = 0.0f;
	if (toEnemy.LengthSquared() < 0.0001f) return;
	toEnemy = toEnemy.Normalize();

	float dot = std::clamp(aimDirection_.Dot(toEnemy), -1.0f, 1.0f);
	float angle = std::acos(dot);

	float threshold = aimAssistAngle_ * (std::numbers::pi_v<float> / 180.0f);
	if (angle > threshold) return;

	aimDirection_ = Vec3::Slerp(aimDirection_, toEnemy, aimAssistStrength_).Normalize();
}

void ShootState::Fire(Player* player)
{
	Vector3 position = player->GetTranslate();

	float bulletSpeed = Tako::GameSettings::Get<PlayerSettings>().shoot.bulletSpeed;
	if (bulletSpeed <= 0.0f) {
		bulletSpeed = 30.0f;  // デフォルト値
	}
	Vector3 velocity = aimDirection_ * bulletSpeed;

	player->RequestBulletSpawn(position, velocity);
}

void ShootState::DrawImGui(Player* player)
{
#ifdef _DEBUG
	ImGui::Text("=== Shoot State Details ===");
	ImGui::Separator();

	ImGui::Text("Fire Rate: %.2f (%.1f shots/sec)", GetFireRate(), 1.0f / GetFireRate());
	ImGui::Text("Next Shot In: %.2f", std::max<float>(0.0f, GetFireRate() - GetFireRateTimer()));

	float progress = (GetFireRate() > 0.0f) ? (GetFireRateTimer() / GetFireRate()) : 0.0f;
	ImGui::ProgressBar(progress, ImVec2(-1, 0), "Reload");

	if (ImGui::TreeNode("Aim Direction")) {
		const Tako::Vector3& aim = GetAimDirection();
		ImGui::Text("X: %.3f", aim.x);
		ImGui::Text("Y: %.3f", aim.y);
		ImGui::Text("Z: %.3f", aim.z);
		ImGui::Text("Length: %.3f", aim.Length());
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Shooting Parameters")) {
		float fireRate = GetFireRate();
		if (UI::SliderFloat("Fire Rate", &fireRate, 0.05f, 2.0f, "%.2f sec")) SetFireRate(fireRate);

		if (ImGui::Button("Rapid Fire")) {
			SetFireRate(0.05f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Normal")) {
			SetFireRate(0.2f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Slow")) {
			SetFireRate(0.5f);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Aim Assist")) {
		// 値は Engine Settings > Game > Player の Shoot で変える
		ImGui::Text("Enabled: %s  Angle: %.1f deg  Strength: %.2f", aimAssistEnabled_ ? "true" : "false", aimAssistAngle_, aimAssistStrength_);

		Boss* boss = player->GetTargetEnemy();
		Tako::Vector3 toEnemy = boss ? boss->GetTranslate() - player->GetTranslate() : Tako::Vector3();
		toEnemy.y = 0.0f;
		if (boss && !boss->IsDead() && toEnemy.LengthSquared() >= 0.0001f) {
			float dot = std::clamp(GetAimDirection().Dot(toEnemy.Normalize()), -1.0f, 1.0f);
			ImGui::Text("Angle To Enemy: %.1f deg", std::acos(dot) * (180.0f / std::numbers::pi_v<float>));
		} else {
			ImGui::Text("Angle To Enemy: --");
		}

		ImGui::TreePop();
	}
#endif
}