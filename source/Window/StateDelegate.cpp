#include "StateDelegate.h"

LAME_BEGIN

void StateDelegate::onStateAttach() {
	this->Disable(Flag::Attach);
}

void StateDelegate::onStateDetach() {
	this->Disable(Flag::Detach);
}

void StateDelegate::onStateRender() {
	this->Disable(Flag::Render);
}

void StateDelegate::onStateChange(StatePtr state) {
	this->Disable(Flag::Change);
}

void StateDelegate::onStateLoad() {
	this->Disable(Flag::Load);
}

void StateDelegate::onStateUnLoad() {
	this->Disable(Flag::UnLoad);
}

void StateDelegate::onStateSet(StatePtr left, StatePtr right) {
	this->Disable(Flag::Set);
}

LAME_END