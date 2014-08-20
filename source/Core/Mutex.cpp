#include "Mutex.h"

#include <pthread.h>
#include <stdlib.h>

LAME_BEGIN2(Core)

Mutex::Mutex() {

	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_NORMAL);
	if (pthread_mutex_init((pthread_mutex_t*)&this->handle, NULL) != 0) {
		throw MutexException("Unable to create mutex");
	}
	pthread_mutexattr_destroy(&attributes);
}

Mutex::~Mutex() {

	if (pthread_mutex_destroy((pthread_mutex_t*)&this->handle) != 0) {
		throw MutexException("Unable to destroy mutex");
	}
}

Void Mutex::Lock(Void) {

	if (pthread_mutex_lock((pthread_mutex_t*)&this->handle) != 0) {
		throw MutexException("Unable to lock mutex");
	}
}

Void Mutex::UnLock(Void) {

	if (pthread_mutex_unlock((pthread_mutex_t*)&this->handle) != 0) {
		throw MutexException("Unable to unlock mutex");
	}
}

Bool Mutex::TryLock(Void) {

	return pthread_mutex_trylock(
		(pthread_mutex_t*)&this->handle) == 0;
}

ConditionPtr Mutex::NewCondition() {
	return new Condition(this);
}

LAME_END2
