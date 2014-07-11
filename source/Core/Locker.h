#ifndef __LAME_CORE_LOCKER__
#define __LAME_CORE_LOCKER__

#include "Types.h"
#include "List.h"

LAME_BEGIN2(Core)

class Locker {
public:
	virtual Bool TryLock() = 0;
	virtual Void Lock() = 0;
	virtual Void UnLock() = 0;
public:
	inline VoidP GetHandle() {
		return this->handle;
	}
protected:
	VoidP handle;
};

LAME_END2

#endif // ~__LAME_CORE_LOCKER__