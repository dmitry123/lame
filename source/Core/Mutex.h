#ifndef __LAME_CORE__MUTEX__
#define __LAME_CORE__MUTEX__

#include "Types.h"
#include "Condition.h"
#include "List.h"
#include "Locker.h"
#include "Exception.h"

LAME_BEGIN2(Core)

class LAME_API Mutex : public Locker {
public:
	Bool TryLock() override;
	Void Lock() override;
	Void UnLock() override;
public:
	ConditionPtr NewCondition();
public:
	Mutex();
	~Mutex();
};

LAME_END2

#endif // ~__LAME_CORE__MUTEX__