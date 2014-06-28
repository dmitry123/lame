#ifndef __LAME_CORE__THREAD__
#define __LAME_CORE__THREAD__

#include "Types.h"
#include "Condition.h"

LAME_BEGIN2(Core)

class LAME_API Thread {
public:
	typedef Void(*ThreadProc)(VoidP);
	typedef Uint64 Handle;
public:
	~Thread();
public:
	Void Create(ThreadProc callback, VoidP parameter);
	Bool Wait(Void);
	Bool Terminate(Void);
	Bool Suspend(Void) const;
	Bool Resume(Void) const;
	Void Yield(Void) const;
	Bool IsAlive(Void) const;
	Bool Equal(const Thread& thread) const;
	Void Sleep(Clock delay) const;
public:
	inline Bool operator == (const Thread& thread) const {
		return this->Equal(thread);
	}
	inline Bool operator != (const Thread& thread) const {
		return !this->Equal(thread);
	}
public:
	inline VoidP GetParameter() {
		return this->parameter_;
	}
	inline Handle GetHandle() {
		return this->handle_;
	}
	inline ConditionPtr GetCondition() {
		return &this->condition_;
	}
public:
	static Handle GetCurrentHandle(Void);
	static ThreadPtr GetCurrentThread(Void);
private:
	static VoidP __PosixThread(VoidP);
private:
	Handle handle_ = 0;
	ThreadProc callback_ = 0;
	VoidP parameter_ = 0;
	Condition condition_;
	Uint32 flags_ = 0;
};

LAME_END2

#endif // ~__LAME_CORE__THREAD__