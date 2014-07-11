#ifndef __LAME_CORE_OBSERVABLE__
#define __LAME_CORE_OBSERVABLE__

#include "List.h"
#include "Mutex.h"

LAME_BEGIN2(Core)

LAME_CLASS(Observer);
LAME_CLASS(Observable);

class Observer {
public:
	virtual Void Update(ObservablePtr observable) = 0;
};

class Observable {
public:
	virtual inline Void Add(ObserverPtr observer) {
		this->observers.insert(observer);
	}
	virtual inline Void Remove(ObserverPtr observer) {
		this->observers.erase(observer);
	}
	virtual inline Void Notify() {
		for (ObserverPtr observer : this->observers) {
			observer->Update(this);
		}
	}
public:
	virtual inline Void SetChanged(Bool isChanged) {
		this->isChanged = isChanged;
	}
	virtual inline Uint32 CountObservers() {
		return Uint32(this->observers.size());
	}
protected:
	Bool isChanged;
	Set<ObserverPtr> observers;
};

class InterlockedObservable : Observable {
public:
	inline Void Add(ObserverPtr observer) {
		this->mutex.Lock();
		this->observers.insert(observer);
		this->mutex.UnLock();
	}
	inline Void Remove(ObserverPtr observer) {
		this->mutex.Lock();
		this->observers.erase(observer);
		this->mutex.UnLock();
	}
	inline Void Notify() {
		this->mutex.Lock();
		for (ObserverPtr observer : this->observers) {
			observer->Update(this);
		}
		this->mutex.UnLock();
	}
public:
	inline Void SetChanged(Bool isChanged) {
		this->mutex.Lock();
		this->isChanged = isChanged;
		this->mutex.UnLock();
	}
	inline Uint32 CountObservers() {
		Uint32 size;
		this->mutex.Lock();
		size = Uint32(this->observers.size());
		this->mutex.UnLock();
		return size;
	}
private:
	Mutex mutex;
};

LAME_END2

#endif // ~__LAME_CORE_OBSERVABLE__