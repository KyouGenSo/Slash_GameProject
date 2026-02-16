#include "BossStateMachine.h"
#include "BossState.h"
#include "../Boss.h"

const std::string BossStateMachine::kEmptyString;

BossStateMachine::BossStateMachine(Boss* boss)
	: boss_(boss)
{
}

BossStateMachine::~BossStateMachine()
{

}

void BossStateMachine::Update(float deltaTime)
{
	if (currentState_) {
		currentState_->Update(boss_, deltaTime);
	}
}

void BossStateMachine::ChangeState(const std::string& stateName)
{
	auto it = states_.find(stateName);
	if (it == states_.end()) {
		return;
	}

	BossState* newState = it->second.get();
	if (!newState || newState == currentState_) {
		return;
	}

	if (currentState_) {
		currentState_->Exit(boss_);
	}

	currentState_ = newState;
	currentState_->Enter(boss_);
}

void BossStateMachine::RegisterState(const std::string& name, std::unique_ptr<BossState> state)
{
	states_[name] = std::move(state);
}

const std::string& BossStateMachine::GetCurrentStateName() const
{
	if (currentState_) {
		return currentState_->GetName();
	}
	return kEmptyString;
}

BossState* BossStateMachine::GetState(const std::string& name) const
{
	auto it = states_.find(name);
	return (it != states_.end()) ? it->second.get() : nullptr;
}
