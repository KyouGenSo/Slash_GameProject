#include "PlayerStateMachine.h"
#include "PlayerState.h"
#include "../Player.h"
#include <assert.h>
#include <vector>
#include <algorithm>

PlayerStateMachine::PlayerStateMachine(Player* player)
	: player_(player)
	, currentState_(nullptr)
	, previousState_(nullptr)
{
}

PlayerStateMachine::~PlayerStateMachine()
{
	if (currentState_)
	{
		currentState_->Exit(player_);
	}
}

void PlayerStateMachine::Initialize()
{
	if (!states_.empty() && !currentState_)
	{
		currentState_ = states_.begin()->second.get();
		currentState_->Enter(player_);
	}
}

void PlayerStateMachine::Update(float deltaTime)
{
	if (currentState_)
	{
		currentState_->Update(player_, deltaTime);
	}
}

void PlayerStateMachine::HandleInput()
{
	if (currentState_)
	{
		currentState_->HandleInput(player_);
	}
}

void PlayerStateMachine::ChangeState(const std::string& stateName)
{
	auto it = states_.find(stateName);
	if (it != states_.end())
	{
		ChangeState(it->second.get());
	}
}

void PlayerStateMachine::ChangeState(PlayerState* newState)
{
	if (!newState || newState == currentState_)
	{
		return;
	}

	if (currentState_ && !currentState_->CanTransitionTo(newState->GetName()))
	{
		return;
	}

	if (currentState_)
	{
		currentState_->Exit(player_);
	}

	previousState_ = currentState_;
	currentState_ = newState;
	currentState_->Enter(player_);
}

void PlayerStateMachine::RegisterState(const std::string& name, std::unique_ptr<PlayerState> state)
{
	states_[name] = std::move(state);
}

PlayerState* PlayerStateMachine::GetState(const std::string& name) const
{
	auto it = states_.find(name);
	return (it != states_.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> PlayerStateMachine::GetAllStateNames() const
{
	std::vector<std::string> names;
	names.reserve(states_.size());

	for (const auto& [name, state] : states_) {
		names.push_back(name);
	}

	// ソートして一定の順序を保証
	std::sort(names.begin(), names.end());

	return names;
}