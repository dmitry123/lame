#ifndef __LAME_CORE_OBSERVABLE__
#define __LAME_CORE_OBSERVABLE__

#include "List.h"
#include "Locker.h"

LAME_BEGIN2(Core)

template <class T> class Observer {
public:
	virtual Void Update(T* observable) = 0;
};

template <class T> class Observable {
protected:
	typedef Observer<T>* Essence;
public:
	virtual inline Void Add(Essence observer) {
		if (observer != NULL) {
			this->observers.insert(observer);
		}
	}
	virtual inline Void Remove(Essence observer) {
		if (observer != NULL) {
			this->observers.erase(observer);
		}
	}
	virtual inline Void Notify(Bool setChanged = FALSE) {
		if (!this->isChanged && setChanged) {
			this->SetChanged(setChanged);
		}
		if (this->isChanged) {
			for (Essence observer : this->observers) {
				observer->Update((T*)this);
			}
		}
	}
public:
	virtual inline Void SetChanged(Bool isChanged) {
		this->isChanged = isChanged;
	}
	virtual inline Uint32 Size() {
		return Uint32(this->observers.size());
	}
public:
	inline Observable<T>* GetObservable() {
		return this;
	}
private:
	Bool isChanged = FALSE;
	Set<Essence> observers;
};

template <class T> class InterlockedObservable : Observable < T > {
	typedef Observable<T>::Essence Essence;
public:
	inline Void Add(Essence observer) {
		this->observerLocker->Lock();
		observable.Add(observer);
		this->observerLocker->UnLock();
	}
	inline Void Remove(Essence observer) {
		this->observerLocker->Lock();
		observable.Remove(observer);
		this->observerLocker->UnLock();
	}
	inline Void Notify() {
		this->observerLocker->Lock();
		observable.Notify();
		this->observerLocker->UnLock();
	}
public:
	inline Void SetChanged(Bool isChanged) {
		this->observerLocker->Lock();
		observable.SetChanged(isChanged);
		this->observerLocker->UnLock();
	}
	inline Uint32 CountObservers() {
		Uint32 size;
		this->observerLocker->Lock();
		size = observable.Size();
		this->observerLocker->UnLock();
		return size;
	}
public:
	InterlockedObservable(LockerPtr locker) :
		observerLocker(locker)
	{
	}
	~InterlockedObservable() {
		this->observerLocker->UnLock();
	}
public:
	inline LockerPtr GetLocker() {
		return this->observerLocker;
	}
private:
	LockerPtr observerLocker;
	Observable<T> observable;
};

LAME_END2

#endif // ~__LAME_CORE_OBSERVABLE__