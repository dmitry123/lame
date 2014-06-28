#ifndef __LAME_CORE__MUTEX__
#define __LAME_CORE__MUTEX__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API Mutex {
public:
	Bool Lock(Void);
	Bool UnLock(Void);
	Bool TryLock(Void);
public:
	Mutex();
	~Mutex();
private:
	VoidP handle_;
};

LAME_END2

#endif // ~__LAME_CORE__MUTEX__