#ifndef __LAME_CORE_WAIT_MANAGER__
#define __LAME_CORE_WAIT_MANAGER__

#include "Types.h"
#include "Thread.h"
#include "Timer.h"
#include "Exception.h"

LAME_BEGIN2(Core)

class LAME_API WaitManager {
	friend class Timer;
private:
	typedef Timer::Proc Proc;
public:
	static WaitManagerPtr GetDefaultWaitManager() {
		return defaultWaitManager;
	}
public:
	Void Append(Proc callback, Uint32 interval);
	Void Remove(TimerPtr timer);
public:
	inline ThreadPtr GetThread() {
		return &this->thread;
	}
	inline MutexPtr GetMutex() {
		return &this->mutex;
	}
	inline ConditionPtr GetCondition() {
		return this->condition;
	}
	inline Clock GetLowestInterval() {
		return this->lowestInterval;
	}
	inline Map<Clock, Set<TimerPtr>>& GetTimerMap() {
		return this->timerList;
	}
private:
	Void _Append(TimerPtr timer);
	Void _Remove(TimerPtr timer);
public:
	WaitManager();
	~WaitManager();
private:
	static WaitManagerPtr defaultWaitManager;
private:
	Thread thread;
	Map<Clock, Set<TimerPtr>> timerList;
	Mutex mutex;
	ConditionPtr condition;
	Uint64 lowestInterval;
};

LAME_END2

#endif // ~__LAME_CORE_WAIT_MANAGER__