#ifndef __LAME_CORE__RECURSIVE_MUTEX__
#define __LAME_CORE__RECURSIVE_MUTEX__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API RecursiveMutex {
public:
	Bool Lock(Void);
	Bool UnLock(Void);
	Bool TryLock(Void);
public:
	RecursiveMutex();
	~RecursiveMutex();
private:
	VoidP handle_;
};

LAME_END2

#endif // ~__LAME_CORE__RECURSIVE_MUTEX__