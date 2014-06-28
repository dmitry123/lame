#include "RecursiveMutex.h"

#include <pthread.h>
#include <stdlib.h>

LAME_BEGIN2(Core)

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

LAME_END2
