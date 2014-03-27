#include "Thread.h"
#include "Utility.h"

#ifdef LAME_WINDOWS
#  include <Windows.h>
#endif

#include <pthread.h>
#include <stdlib.h>

LAME_BEGIN

// critical section

CriticalSection::CriticalSection() {
#ifdef LAME_WINDOWS
	this->handle_ = ZeroAlloc(sizeof (CRITICAL_SECTION));
	InitializeCriticalSection((PCRITICAL_SECTION)this->handle_);
#else
	pthread_mutex_init((pthread_mutex_t*)&this->handle_, NULL);
#endif
}

CriticalSection::~CriticalSection() {
#ifdef LAME_WINDOWS
	DeleteCriticalSection((PCRITICAL_SECTION)this->handle_);
	Free(this->handle_);
#else
	pthread_mutex_destroy((pthread_mutex_t*)&this->handle_);
#endif
}

Void CriticalSection::Lock(Void) {
#ifdef LAME_WINDOWS
	EnterCriticalSection((PCRITICAL_SECTION)this->handle_);
#else
	pthread_mutex_lock((pthread_mutex_t*)&this->handle_);
#endif
}

Void CriticalSection::UnLock(Void) {
#ifdef LAME_WINDOWS
	LeaveCriticalSection((PCRITICAL_SECTION)this->handle_);
#else
	pthread_mutex_unlock((pthread_mutex_t*)&this->handle_);
#endif
}

Bool CriticalSection::TryLock(Void) {
	return
#ifdef LAME_WINDOWS
		TryEnterCriticalSection((PCRITICAL_SECTION)this->handle_);
#else
		pthread_mutex_trylock((pthread_mutex_t*)&this->handle_);
#endif
}

// normal mutex

Mutex::Mutex() {

	pthread_mutexattr_t attributes;

	// initialize posix mutex
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_NORMAL);
	pthread_mutex_init((pthread_mutex_t*)&this->handle_, &attributes);
	pthread_mutexattr_destroy(&attributes);
}

Mutex::~Mutex() {

	// destroy posix mutex
	pthread_mutex_destroy(
		(pthread_mutex_t*)&this->handle_);
}

Bool Mutex::Lock(Void) {

	// lock posix mutex
	return pthread_mutex_lock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

Bool Mutex::UnLock(Void) {

	// unlock posix mutex
	return pthread_mutex_unlock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

Bool Mutex::TryLock(Void) {

	// try to lock posix mutex
	return pthread_mutex_trylock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

// recursive mutex

RecursiveMutex::RecursiveMutex() {

	pthread_mutexattr_t attributes;

	// initialize posix recursive mutex
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init((pthread_mutex_t*)&this->handle_, &attributes);
	pthread_mutexattr_destroy(&attributes);
}

RecursiveMutex::~RecursiveMutex() {

	// destroy posix recursive mutex
	pthread_mutex_destroy(
		(pthread_mutex_t*)&this->handle_);
}

Bool RecursiveMutex::Lock(Void) {

	// lock posix recursive mutex
	return pthread_mutex_lock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

Bool RecursiveMutex::UnLock(Void) {

	// unlock posix recursive mutex
	return pthread_mutex_unlock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

Bool RecursiveMutex::TryLock(Void) {

	// try to lock posix recursive mutex
	return pthread_mutex_trylock(
		(pthread_mutex_t*)&this->handle_) == 0;
}

LAME_END