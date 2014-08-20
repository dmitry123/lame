#include "Condition.h"
#include "Mutex.h"

#include <pthread.h>
#include <errno.h>

LAME_BEGIN2(Core)

Condition::Condition(MutexPtr mutex) {
	this->mutex = mutex;
	pthread_cond_init(&this->condition_, NULL);
}

Condition::~Condition() {
	pthread_cond_destroy(&this->condition_);
}

Void Condition::Lock(Void) {
	this->mutex->Lock();
}

Void Condition::UnLock(Void) {
	this->mutex->UnLock();
}

Bool Condition::Signal(Void) {
	return pthread_cond_signal(
        &this->condition_) == 0;
}

Bool Condition::Broadcast(Void) {
	return pthread_cond_broadcast(
        &this->condition_) == 0;
}

Bool Condition::Wait(Void) {

	pthread_mutex_t mutexHandle = this->mutex->GetHandle();

	return pthread_cond_wait(
        &this->condition_, &mutexHandle) == 0;
}

Bool Condition::TimedWait(Clock duration) {

	Sint32 result;
	timespec s;

	pthread_mutex_t mutexHandle = this->mutex->GetHandle();

#ifdef LAME_WINDOWS
    s.tv_sec = time(NULL) + duration / 1000;
#else
    s.tv_sec = __darwin_time_t(time(NULL) + duration / 1000);
#endif
    
	s.tv_nsec = 1000000 * (duration % 1000);

	result = pthread_cond_timedwait(
		(pthread_cond_t*)&this->condition_,
		(pthread_mutex_t*)&mutexHandle, &s);

	return result == ETIMEDOUT;
}

Void Condition::Release(Void) {

	this->Broadcast();
	delete this;
}

LAME_END2
