#ifndef __LAME_CORE__CONDITION__
#define __LAME_CORE__CONDITION__

#include "Define.h"
#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API Condition {
public:
	Bool Lock(Void);
	Bool UnLock(Void);
	Bool Signal(Void);
	Bool Broadcast(Void);
	Bool Wait(Void);
	Bool TimedWait(Clock duration);
public:
	Condition();
	~Condition();
private:
	VoidP lock_;
	VoidP condition_;
};

LAME_END2

#endif // ~__LAME_CORE__CONDITION__