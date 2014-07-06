#include "State.h"
#include "StateMachine.h"

LAME_BEGIN

State::State(StringC name) :
	stateName(name),
	stateMachine(NULL)
{
	this->nextState = NULL;
	this->prevState = NULL;
	this->isAllocated = FALSE;
}

State::~State() {
	if (this->stateMachine != NULL) {
		this->stateMachine->Detach(this);
	}
}

Void State::Next() {
	this->stateMachine->Change(this->nextState);
}

Void State::Prev() {
	this->stateMachine->Change(this->prevState);
}

Void State::_SetStateMachine(StateMachinePtr stateMachine) {
	this->stateMachine = stateMachine;
}

Void State::_RefreshState(Void) {

	nextState = NULL;
	prevState = NULL;
	stateName = NULL;
	stateMachine = NULL;
	isAllocated = FALSE;
}

LAME_END