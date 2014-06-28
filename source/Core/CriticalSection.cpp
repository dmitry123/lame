#include "CriticalSection.h"
#include "Utility.h"

#ifdef LAME_WINDOWS
#  include <Windows.h>
#endif

LAME_BEGIN2(Core)

CriticalSection::CriticalSection() {
#ifdef LAME_WINDOWS
	this->handle_ = ZeroAlloc(sizeof(CRITICAL_SECTION));
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

LAME_END2
