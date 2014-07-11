#ifndef __LAME_CORE_TIMER__
#define __LAME_CORE_TIMER__

#include "Mutex.h"

LAME_BEGIN2(Core)

class LAME_API Timer : public Locker {
public:
	typedef Void(*Proc)(TimerPtr timer);
public:
	Void Reset(Void);
	Void Terminate(Void);
	Bool Update(Void);
public:
	inline Bool TryLock() override {
		return this->mutex.TryLock();
	}
	inline Void Lock() override {
		this->mutex.Lock();
	}
	inline Void UnLock() override {
		this->mutex.UnLock();
	}
public:
	inline Proc GetCallback() {
		return this->callback;
	}
	inline Clock GetInterval() {
		return this->timeInterval;
	}
	inline Void SetInterval(Clock interval) {
		this->timeInterval = interval;
	}
	inline Bool IsSuspended() {
		return this->isSuspended;
	}
	inline Void SetSuspended(Bool isSuspended) {
		this->isSuspended = isSuspended;
	}
	inline WaitManagerPtr GetWaitManager() {
		return this->waitManager;
	}
	inline Uint32 GetRepeats() {
		return this->repeats;
	}
public:
	Timer(Proc timerCallback, Clock timeInterval,
		WaitManagerPtr waitManager = NULL);
public:
	~Timer() {
		this->mutex.UnLock();
	}
private:
	Proc callback;
	Clock timeInterval;
	Clock lastTime;
	Bool isSuspended;
	WaitManagerPtr waitManager;
	Mutex mutex;
	Uint32 repeats;
};

LAME_END2

#endif // ~__LAME_CORE_TIMER__