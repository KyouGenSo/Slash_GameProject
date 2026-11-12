#include "DashState.h"
#include "PlayerStateMachine.h"
#include "Script/Object/Player/Player.h"
#include "Source/Input/InputHandler.h"
#include "Source/Settings/PlayerSettings.h"
#ifdef _DEBUG
#include "EditorUI.h"
#endif

using namespace Tako;

void DashState::Enter(Player* player)
{
	// TODO: アニメーション作成後に実装
	// player->GetModel()->PlayAnimation("Dash");

	timer_ = 0.0f;
	player->SetInvincible(true);  // ダッシュ中は無敵
}

void DashState::Update(Player* player, float deltaTime)
{
	const PlayerSettings::Dash& dash = Tako::GameSettings::Get<PlayerSettings>().dash;
	duration_ = dash.duration;
	speed_ = dash.speed;

	timer_ += deltaTime;

	player->Move(deltaTime, speed_);

	if (timer_ >= duration_)
	{
		PlayerStateMachine* stateMachine = player->GetStateMachine();
		if (stateMachine)
		{
			// 移動入力があれば Move、なければ Idle
			InputHandler* input = player->GetInputHandler();
			if (input && input->IsMoving())
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

void DashState::Exit(Player* player)
{
	player->SetInvincible(false);
	timer_ = 0.0f;

	player->StartDashCooldown();
}

void DashState::HandleInput(Player* player)
{
	// ダッシュ中は入力を受け付けない
}

void DashState::DrawImGui(Player* player)
{
#ifdef _DEBUG
	ImGui::Text("=== Dash State Details ===");
	ImGui::Separator();

	float progress = (GetDuration() > 0.0f) ? (GetTimer() / GetDuration()) : 0.0f;
	ImGui::Text("Dash Progress: %.1f%%", progress * 100.0f);
	ImGui::ProgressBar(progress);

	ImGui::Text("Timer: %.3f / %.3f", GetTimer(), GetDuration());

	if (ImGui::TreeNode("Dash Parameters")) {
		float duration = GetDuration();
		if (UI::SliderFloat("Duration", &duration, 0.01f, 0.5f, "%.3f")) SetDuration(duration);
		float speed = GetSpeed();
		if (UI::SliderFloat("Speed", &speed, 1.0f, 50.0f, "%.1f")) SetSpeed(speed);

		if (ImGui::Button("Fast Dash")) {
			SetDuration(0.05f);
			SetSpeed(10.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Long Dash")) {
			SetDuration(0.2f);
			SetSpeed(5.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Teleport")) {
			SetDuration(0.01f);
			SetSpeed(50.0f);
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Debug Info")) {
		ImGui::Text("Actual Distance: %.2f", GetTimer() * GetSpeed());
		ImGui::Text("Frame Count: %d", (int)(GetTimer() * 60.0f));
		ImGui::TreePop();
	}
#endif
}