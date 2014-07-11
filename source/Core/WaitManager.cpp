#include "WaitManager.h"

LAME_BEGIN2(Core)

WaitManagerPtr WaitManager::defaultWaitManager
	= new WaitManager();

Void WaitManager::Append(Proc callback, Uint32 interval) {
	new Timer(callback, interval, this);
}

Void WaitManager::Remove(TimerPtr timer) {
	this->_Remove(timer);
}

Void WaitManager::_Append(TimerPtr timer) {

	if (!this->timerList.count(timer->GetInterval())) {
		this->timerList.insert(std::make_pair(timer->GetInterval(), Set<TimerPtr>()));
	}

	Set<TimerPtr>& timerSet = this->timerList.at(timer->GetInterval());

	if (timerSet.count(timer)) {
		throw WaitManagerException("WaitManager already has this timer");
	}

	timerSet.insert(timer);

	if (timer->GetInterval() < this->lowestInterval) {
		this->lowestInterval = timer->GetInterval();
	}

	if (!this->thread.IsAlive()) {
		this->thread.Create([] (VoidP waitManager_) {

			WaitManagerPtr waitManager = WaitManagerPtr(waitManager_);
			ConditionPtr condition = waitManager->GetCondition();

			while (TRUE) {

				condition->Lock();
				if (!waitManager->GetTimerMap().size()) {
					condition->TimedWait(LAME_INFINITE);
				}
				else {
					condition->TimedWait(waitManager->GetLowestInterval());
				}
				condition->UnLock();

				for (auto i : waitManager->GetTimerMap()) {
					for (TimerPtr t : i.second) {
						if (!t->Update()) {
							goto _Break;
						}
						t->GetCallback()(t);
					}
				}
			_Break:
				;
			}
		}, this);
	}
	else {
		this->condition->Broadcast();
	}
}

Void WaitManager::_Remove(TimerPtr timer) {

	if (!this->timerList.count(timer->GetInterval())) {
		throw WaitManagerException("WaitManager hasn't timers with that interval");
	}

	Set<TimerPtr>& timerSet = this->timerList.at(timer->GetInterval());

	if (!timerSet.count(timer)) {
		throw WaitManagerException("WaitManager hasn't this timer in set");
	}

	timerSet.erase(timer);

	if (!timerSet.size()) {
		this->timerList.erase(timer->GetInterval());
	}

	delete timer;
}

WaitManager::WaitManager() {
	this->condition = this->mutex.NewCondition();
	this->lowestInterval = Uint32(-1);
}

WaitManager::~WaitManager() {
	this->condition->Release();
}

LAME_END2