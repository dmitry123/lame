#ifndef __LAME_CORE__THREAD__
#define __LAME_CORE__THREAD__

#include "Define.h"
#include "Types.h"

LAME_BEGIN

typedef class CriticalSection CriticalSection, *CriticalSectionPtr;
typedef class Mutex Mutex, *MutexPtr;
typedef class RecursiveMutex RecursiveMutex, *RecursiveMutexPtr;
typedef class Condition Condition, *ConditionPtr;
typedef class Thread Thread, *ThreadPtr;

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

class LAME_API Key {
public:
	typedef Void(*KeyProc)(VoidP);
public:
	Bool Set(VoidP value);
	VoidP Get(Void);
public:
	Key();
	~Key();
public:
	Key(KeyProc destructor);
private:
	VoidP handle_;
};

class LAME_API Thread {
public:
	typedef Void(*ThreadProc)(VoidP);
	typedef Uint64 Handle;
public:
	Void Create(ThreadProc callback, VoidP parameter);
	Bool Wait(Void);
	Bool Terminate(Void);
	Bool Suspend(Void) const;
	Bool Resume(Void) const;
	Void Yield(Void) const;
	Bool IsAlive(Void) const;
	Bool Equal(const Thread& thread) const;
public:
	inline Bool operator == (const Thread& thread) const {
		return this->Equal(thread);
	}
	inline Bool operator != (const Thread& thread) const {
		return !((*this) == thread);
	}
public:
	inline VoidP GetParameter() {
		return this->parameter_;
	}
	inline Handle GetHandle() {
		return this->handle_;
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

LAME_END

#endif // ~__LAME_CORE__THREAD__