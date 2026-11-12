#include "PlayerState.h"
#include "PlayerStateMachine.h"

void PlayerState::ChangeState(PlayerStateMachine* stateMachine, PlayerState* newState)
{
	if (stateMachine && newState)
	{
		stateMachine->ChangeState(newState);
	}
}