#ifndef __LAME_CORE__CONDITION__
#define __LAME_CORE__CONDITION__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API Condition {
	friend class Mutex;
public:
	Void Lock(Void);
	Void UnLock(Void);
	Bool Signal(Void);
	Bool Broadcast(Void);
	Bool Wait(Void);
	Bool TimedWait(Clock duration);
	Void Release(Void);
private:
	Condition(MutexPtr mutex);
	~Condition();
private:
	MutexPtr mutex;
	VoidP condition_;
};

LAME_END2

#endif // ~__LAME_CORE__CONDITION__