#include "Thread.h"

#include <pthread.h>
#include <errno.h>

#ifdef LAME_MSVC
#  ifdef LAME_X64
#    pragma comment(lib, "x64/pthreadvc2.lib")
#  else
#    pragma comment(lib, "x86/pthreadvc2.lib")
#  endif
#endif

LAME_BEGIN

Condition::Condition() {
	pthread_cond_init((pthread_cond_t*)&this->condition_, NULL);
	pthread_mutex_init((pthread_mutex_t*)&this->lock_, NULL);
}

Condition::~Condition() {
	pthread_mutex_destroy((pthread_mutex_t*)&this->lock_);
	pthread_cond_destroy((pthread_cond_t*)&this->condition_);
}

Bool Condition::Lock(Void) {
	return pthread_mutex_lock(
		(pthread_mutex_t*)&this->lock_) == 0;
}

Bool Condition::UnLock(Void) {
	return pthread_mutex_unlock(
		(pthread_mutex_t*)&this->lock_) == 0;
}

Bool Condition::Signal(Void) {
	return pthread_cond_signal(
		(pthread_cond_t*)&this->condition_) == 0;
}

Bool Condition::Broadcast(Void) {
	return pthread_cond_broadcast(
		(pthread_cond_t*)&this->condition_) == 0;
}

Bool Condition::Wait(Void) {
	return pthread_cond_wait(
			(pthread_cond_t*)&this->condition_,
			(pthread_mutex_t*)&this->lock_) == 0;
}

Bool Condition::TimedWait(Clock duration) {

	Sint32 result;
	timespec s;

#ifdef LAME_WINDOWS
    s.tv_sec = time(NULL) + duration / 1000;
#else
    s.tv_sec = __darwin_time_t(time(NULL) + duration / 1000);
#endif
    
	s.tv_nsec = 1000000 * (duration % 1000);

	result = pthread_cond_timedwait(
		(pthread_cond_t*)&this->condition_,
		(pthread_mutex_t*)&this->lock_, &s);

	return result == ETIMEDOUT;
}

LAME_END