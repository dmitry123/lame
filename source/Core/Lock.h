#ifndef __LAME_CORE__LOCK__
#define __LAME_CORE__LOCK__

#include "Define.h"

LAME_BEGIN2(Core)

class LockNew {
protected:
	inline void* operator new(size_t size) {
		return ::operator new(size);
	}
	inline void* operator new[](size_t size) {
		return ::operator new[](size);
	}
};

class LockDelete {
protected:
	inline void operator delete(void* p) {
		return ::operator delete(p);
	}
	inline void operator delete[](void* p) {
		return ::operator delete[](p);
	}
};

class LockCopy {
protected:
	 LockCopy() = default;
	~LockCopy() = default;
protected:
	LockCopy(const LockCopy&) = delete;
protected:
	LockCopy& operator = (const LockCopy&) = delete;
};

LAME_END2

#endif // ~__LAME_CORE__LOCK__