#include "Thread.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#ifdef LAME_WINDOWS
#  include <Windows.h>
#else
#  include <unistd.h>
#endif

#ifndef LAME_WINDOWS
#  include <sys/types.h>
#endif

#include <errno.h>
#include <sched.h>

#ifdef LAME_WINDOWS
#  undef Yield
#endif

LAME_BEGIN

namespace internal {
	typedef class ThreadNode : public Thread {
		typedef ThreadNode* ThreadNodePtr;
	public:
		ThreadNodePtr next = 0;
	} *ThreadNodePtr;

	ThreadNodePtr firstNode = 0;
	ThreadNodePtr lastNode = 0;
}

Void Thread::Create(ThreadProc callback, VoidP parameter) {

	pthread_attr_t thread_attributes;

	this->callback_ = callback;
	this->parameter_ = parameter;

	pthread_attr_init(&thread_attributes);
	pthread_attr_setdetachstate(&thread_attributes, PTHREAD_CREATE_DETACHED);
	pthread_create((pthread_t*)&this->handle_, &thread_attributes, __PosixThread, this);
	pthread_attr_destroy(&thread_attributes);
	pthread_detach(*(pthread_t*)&this->handle_);

	if (!internal::firstNode) {
		internal::firstNode = (internal::ThreadNodePtr)this;
		internal::lastNode = internal::firstNode;
		internal::firstNode->next = 0;
	}
	else {
		internal::lastNode->next = (internal::ThreadNodePtr)this;
		internal::lastNode = internal::lastNode->next;
		internal::lastNode->next = 0;
	}
}

Bool Thread::Wait(Void) {

	Sint32 result = 0;

	if (!this->handle_) {
		return 0;
	}

	do {} while (this->condition_.TimedWait(1000) != ETIMEDOUT);

	return result == 0;
}

Bool Thread::Terminate(Void) {

	if (!this->handle_) {
		return 0;
	}

	this->condition_.Broadcast();

	return pthread_cancel(*(pthread_t*)&this->handle_) == 0;
}

Bool Thread::Suspend(Void) const {

#ifdef LAME_WINDOWS
	VoidP handle;
#endif

	if (!this->handle_) {
		return LAME_FALSE;
	}

#ifdef LAME_WINDOWS

	handle = pthread_getw32threadhandle_np(*(pthread_t*)&this->handle_);

	if (!handle) {
		return LAME_FALSE;
	}

	return SuspendThread(handle) != -1;

#else

	return pthread_kill(*(pthread_t*)&this->handle_, SIGSTOP) == 0;
#endif

	return 1;
}

Bool Thread::Resume(Void) const {

#ifdef LAME_WINDOWS
	VoidP handle;
#endif

	if (!this->handle_) {
		return LAME_FALSE;
	}

#ifdef LAME_WINDOWS

	handle = pthread_getw32threadhandle_np(*(pthread_t*)&this->handle_);

	if (!handle) {
		return LAME_FALSE;
	}

	return ResumeThread(handle) != -1;

#else
	return pthread_kill(*(pthread_t*)&this->handle_, SIGSTOP) == 0;
#endif
}	

Void Thread::Yield(Void) const {

#ifdef LAME_WINDOWS
#  if _WIN32_WINNT >= 0x0400
	SwitchToThread();
#  else
	Sleep(0);
#  endif
#else
	sched_yield();
#endif
}

Bool Thread::IsAlive(Void) const {

#ifdef LAME_WINDOWS
	HANDLE h = ::OpenThread(SYNCHRONIZE, FALSE, GetThreadId(
		pthread_getw32threadhandle_np(*(pthread_t*)&this->handle_)));

	if (h == NULL) {
		return FALSE;
	}

	::CloseHandle(h);

	return TRUE;
#else
	return pthread_kill( *(pthread_t*)&this->handle_, 0 ) != ESRCH ;
#endif
}

Bool Thread::Equal(const Thread& thread) const {

	return pthread_equal(
		*(pthread_t*)&this->handle_,
		*(pthread_t*)&thread.handle_);
}

Uint64 Thread::GetCurrentHandle(Void) {
    
    pthread_t handle = pthread_self();
	return *(Uint64P)&handle;
}

ThreadPtr Thread::GetCurrentThread(Void) {

	internal::ThreadNodePtr node = internal::firstNode;
	Handle handle = GetCurrentHandle();

	while (node != internal::lastNode) {
		if (node->handle_ == handle) {
			return node;
		}
		node = node->next;
	}

	return LAME_NULL;
}

VoidP Thread::__PosixThread(VoidP __this) {

	Thread* thread = (Thread*)__this;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	thread->callback_(thread->parameter_);
	thread->condition_.Broadcast();
	thread->handle_ = 0;
	pthread_exit(NULL);
	return LAME_NULL;
}

LAME_END