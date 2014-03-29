#include "Thread.h"

LAME_BEGIN

Thread ThreadPool::GetCurrent(Void) {

	Uint64 handle = CThread::GetCurrent();
	ThreadPool manager;

	if (handle) {
		for (Thread thread : manager->threads_) {
			if (thread->GetHandle() == handle) {
				return thread;
			}
		}
	}

	PostErrorMessage("Unable to find thread in manager", 1);
    
    return manager->threads_.back();
}

Thread ThreadPool::AttachThread(Void) {

	CriticalSectionLock(this->lock_);

	Thread thread;

	this->threads_.push_back(thread);
	this->count_++;

	return thread;
}

Void ThreadPool::DetachThread(Thread thread) {

	CriticalSectionLock(this->lock_);

	List::iterator i;

	i = this->threads_.begin();

	while (*i != thread) {
		i++;
	}

	if ((*i)->Terminate()) {
		// success
	}

	--this->count_;

	this->threads_.erase(i);
}

Void ThreadPool::DetachAndWaitThread(Thread thread) {

	CriticalSectionLock(this->lock_);

	List::iterator i;

	i = this->threads_.begin();

	while (*i != thread) {
		i++;
	}

	if ((*i)->Terminate()) {
		(*i)->Wait();
	}

	--this->count_;

	this->threads_.erase(i);
}

Uint32 ThreadPool::GetCountOfThreads(Void) {

	return
		count_;
}

Void ThreadPool::TerminateEveryThread(Void) {

	CriticalSectionLock(this->lock_);

	for (Thread thread : this->threads_) {
		thread->Terminate();
	}

	this->threads_.clear();
}

Uint32 ThreadPool::WaitEveryThread(Void) {

	Uint32 count = 0;
	List list;

	this->lock_->Lock();
	list = this->threads_;
	this->lock_->UnLock();

	for (Thread thread : list) {
		if (!thread->Wait()) {
			++count;
		}
	}

	return count;
}

LAME_END
