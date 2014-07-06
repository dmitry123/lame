#include "StateMachine.h"

LAME_BEGIN

StateMachine::StateMachine() {

	this->current = NULL;
	this->first = NULL;
	this->last = NULL;
}

StateMachine::~StateMachine() {

	while (this->first) {
		this->Detach(this->first);
	}

	this->current = NULL;
	this->first = NULL;
	this->last = NULL;
}

Void StateMachine::Attach(StatePtr state) {

	if (IsExist(state)) {
		PostErrorMessage("Machine has state with that name (%s)", state->GetName());
	}

	if (this->first == NULL) {
		this->first = this->last = state;
	}
	else {
		this->last->nextState = state;
		state->prevState = this->last;
		this->last = state;
	}

	if (current == NULL) {
		current = state;
	}

	this->hash.insert(std::make_pair(state->GetName(), state));

	state->_SetStateMachine(this);
	state->onStateAttach();
}

Bool StateMachine::Detach(StatePtr state) {

	if (!IsExist(state)) {
		return FALSE;
	}

	if (state == this->first) {
		this->first = this->first->nextState;
	}

	if (state == this->last) {
		this->last = state->prevState;
	}

	if (state->prevState) {
		state->prevState->nextState = state->nextState;
	}

	if (state->nextState) {
		state->nextState->prevState = state->prevState;
	}

	state->onStateDetach();
	this->hash.erase(state->GetName());
	state->_RefreshState();

	if (state->isAllocated) {
		delete state;
	}

	return TRUE;
}

Bool StateMachine::Detach(StringC name) {

	StatePtr state;

	if (!(state = this->Find(name))) {
		return FALSE;
	}

	return this->Detach(state);
}

Bool StateMachine::Change(StatePtr state) {

	if (!this->IsExist(state) || this->current == state) {
		return FALSE;
	}

	this->current->onStateUnLoad();
	this->current->onStateChange(state);

	this->history.push_back(this->current);

	this->current = state;
	this->current->onStateLoad();

	return TRUE;
}

Bool StateMachine::Change(StringC name) {

	StatePtr state;

	if (!(state = this->Find(name))) {
		return FALSE;
	}

	return this->Change(state);
}

StatePtr StateMachine::GetLast(Void) {

	if (!this->history.size()) {
		return NULL;
	}

	return this->history.front();
}

StatePtr StateMachine::PopLast(Void) {

	StatePtr last = this->GetLast();

	if (last != NULL) {
		this->history.pop_front();
	}
	else {
		return NULL;
	}

	return last;
}

Bool StateMachine::Next(Void) {

	if (this->current->nextState) {
		return this->Change(this->current->nextState);
	}

	return FALSE;
}

Bool StateMachine::Prev(Void) {

	if (this->current->prevState) {
		return this->Change(this->current->prevState);
	}

	return FALSE;
}

Bool StateMachine::IsExist(StatePtr with) {

	if (!with || !with->GetName()) {
		return FALSE;
	}

	if (this->hash.count(with->GetName())) {
		return TRUE;
	}

	return FALSE;
}

Bool StateMachine::IsExist(StringC name) {

	StatePtr state;

	if (!(state = this->Find(name))) {
		return FALSE;
	}

	return TRUE;
}

Void StateMachine::Render(Void) {

	if (this->current) {
		this->current->onStateRender();
	}
}

StatePtr StateMachine::Find(StringC name) {

	Map<StringC, StatePtr>::iterator i = this->hash.find(name);

	if (i != this->hash.end()) {
		return i->second;
	}

	return NULL;
}

LAME_END