#include "Mutex.h"

#include <pthread.h>
#include <stdlib.h>

LAME_BEGIN2(Core)

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

LAME_END2
