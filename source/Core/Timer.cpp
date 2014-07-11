#include "Timer.h"
#include "WaitManager.h"
#include "Time.h"

LAME_BEGIN2(Core)

Void Timer::Reset(Void) {
	this->lastTime = 0;
}

Void Timer::Terminate(Void) {
	this->waitManager->Remove(this);
}

Bool Timer::Update(Void) {

	Clock currentTime = Time::GetTime();

	if (currentTime - this->lastTime >= this->timeInterval) {
		this->lastTime = currentTime;
		++this->repeats;
		return TRUE;
	}

	return FALSE;
}

Timer::Timer(Proc timerCallback, Clock timeInterval, WaitManagerPtr waitManager) {

	if (!waitManager) {
		waitManager = WaitManager::GetDefaultWaitManager();
	}
	this->callback = timerCallback;
	this->timeInterval = timeInterval;
	this->waitManager = waitManager;
	this->waitManager->_Append(this);
	this->repeats = 0;
}

LAME_END2