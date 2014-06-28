#include "Timer.h"

LAME_BEGIN2(Core)

Void Timer::Create(Callback callback, Clock interval) {

	this->Reset();

	this->callback_ = callback;
	this->interval_ = interval;
}

Bool Timer::Update(Void) {

	Clock current = Time::GetTime();

	if (this->suspended_) {
		return LAME_TRUE;
	}

	if (current - this->last_ >= this->interval_) {

		// save current time interval as last
		this->last_ = current;

		// invoke callback
		if (this->callback_) {
			this->callback_(this);
		}
	}

	return
		this->interval_ > 0;
}

Void Timer::Terminate(Void) {

	this->last_ = 0;
	this->suspended_ = 0;
	this->interval_ = 0;
}

Void Timer::Suspend(Void) {
	this->suspended_ = LAME_TRUE;
}

Void Timer::Resume(Void) {
	this->suspended_ = LAME_FALSE;
}

Void Timer::Reset(Void) {

	this->last_ = 0;
	this->suspended_ = 0;
}

LAME_END2
