#ifndef __LAME_CORE_LOCKER__
#define __LAME_CORE_LOCKER__

#include "Types.h"
#include "List.h"

#include <pthread.h>

LAME_BEGIN2(Core)

class Locker {
public:
    typedef pthread_mutex_t Handle;
public:
	virtual Bool TryLock() = 0;
	virtual Void Lock() = 0;
	virtual Void UnLock() = 0;
public:
	inline pthread_mutex_t GetHandle() {
		return this->handle;
	}
protected:
    pthread_mutex_t handle;
};

LAME_END2

#endif // ~__LAME_CORE_LOCKER__