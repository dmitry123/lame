#ifndef __LAME_CORE__CRITICAL_SECTION__
#define __LAME_CORE__CRITICAL_SECTION__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API CriticalSection {
public:
	Void Lock(Void);
	Void UnLock(Void);
	Bool TryLock(Void);
public:
	CriticalSection();
	~CriticalSection();
private:
	VoidP handle_;
};

LAME_END2

#endif // ~__LAME_CORE__CRITICAL_SECTION__