#ifndef __LAME_CORE_SHARED_PTR__
#define __LAME_CORE_SHARED_PTR__

#include "Types.h"
#include "Atomic.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stddef.h>
#include <stdarg.h>

#if defined(LAME_APPLE)
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

#include <string>

#include <memory>

LAME_BEGIN2(Core)

inline VoidP Malloc(Uint32 size) {
	return malloc(size);
}

inline Void Free(VoidP pointer) {
	free(pointer);
}

inline VoidP Realloc(VoidP pointer, Uint32 size) {
	return realloc(pointer, size);
}

inline volatile VoidP ZeroMemory(VoidP pointer, Uint32 size) {
	return memset(pointer, 0, size);
}

inline volatile VoidP ZeroAlloc(Uint32 size) {
	return ZeroMemory(Malloc(size), size);
}

typedef struct StrongWeakCounter {
	Atomic32 strong;
	Atomic32 weak;
} *StrongWeakCounterPtr;

namespace internal {
	class __SharedPtrFromThis {
	};
}

template <class T> class SharedPtr {
public:
	inline SharedPtr() {
		this->reference_ = NULL;
		this->counter_ = NULL;
	}
	inline ~SharedPtr() {
		this->_Destroy();
	}
public:
	typedef T Type, *TypeP, &TypeR;
public:
	inline explicit SharedPtr(TypeP reference) : SharedPtr() {
		this->_Create(reference);
	}
	inline SharedPtr(const SharedPtr <T>& sp) : SharedPtr() {
		this->_Copy(sp);
	}
public:
	inline Void operator = (TypeP reference) {
		this->_Create(reference);
	}
	inline Void operator = (const SharedPtr <T>& sp) {
		this->_Copy(sp);
	}
	inline Void operator () (TypeP reference) {
		this->_Create(reference);
	}
protected:
	Void _Create(TypeP reference);
	Void _Destroy();
	Void _Copy(const SharedPtr <T>& sp);
public:
	Void UnLink();
public:
	inline Void reset() {
		this->UnLink();
	}
	inline Void reset(TypeP reference) {
		this->_Create(reference);
	}
public:
	inline TypeP operator -> () {
		return this->reference_;
	}
	inline TypeR operator * () {
		return *this->reference_;
	}
	inline operator Bool() const {
		return this->reference_ != 0;
	}
	inline Bool operator == (const SharedPtr <T>& sp) const {
		return this->reference_ == sp.reference_;
	}
	inline Bool operator != (const SharedPtr <T>& sp) const {
		return this->reference_ != sp.reference_;
	}
private:
	Void _EnableSharedPtr(VoidP ptr) {
		// ignore
	}
	Void _EnableSharedPtr(internal::__SharedPtrFromThis* ptr) {
		((SharedPtrFromThis<T>*) ptr)->weakPtr._Create(*this);
	}
public:
	TypeP reference_;
	StrongWeakCounterPtr counter_;
};

template <class T>
Void SharedPtr <T>::_Create(TypeP reference) {

	if (!reference || !this) {
		return;
	}

	if (this->counter_) {
		this->_Destroy();
	}

	this->reference_ = reference;
	this->counter_ = new StrongWeakCounter();
	this->counter_->strong = 1;
	this->counter_->weak = 0;

	this->_EnableSharedPtr(this->reference_);
}

template <class T>
Void SharedPtr <T>::_Destroy() {

	if (!this) {
		return;
	}

	if ((this->counter_ &&   this->counter_->strong == 0) ||
		(this->counter_ && --this->counter_->strong == 0)) {

		if (this->counter_->weak == 0) {
			delete this->counter_;
		}

		delete this->reference_;

		this->counter_ = 0;
		this->reference_ = 0;
	}
}

template <class T>
Void SharedPtr <T>::_Copy(const SharedPtr <T>& sp) {

	if (!&sp || !this) {
		return;
	}

	if (this->counter_) {
		this->_Destroy();
	}

	this->reference_ = sp.reference_;
	this->counter_ = sp.counter_;

	if (this->counter_) {
		++this->counter_->strong;
	}
}

template <class T>
Void SharedPtr <T>::UnLink() {

	if (!this) {
		return;
	}

	this->__Destroy();

	this->reference_ = 0;
	this->counter_ = 0;
}

template <class T> class WeakPtr : private SharedPtr < T > {
	friend class SharedPtr < T >;
public:
	WeakPtr() {
	}
	~WeakPtr() {
		this->UnLink();
	}
public:
	explicit WeakPtr(const SharedPtr<T>& sp) {
		this->_Create(sp);
	}
	explicit WeakPtr(typename SharedPtr<T>::TypeP st) {
		this->_Create(st);
	}
public:
	inline Void reset() {
		this->UnLink();
	}
public:
	inline Void UnLink() {
		this->_Destroy();
	}
public:
	inline WeakPtr<T> operator = (SharedPtr<T>& sp) {
		return WeakPtr<T>(sp);
	}
	inline WeakPtr<T> operator = (typename SharedPtr<T>::TypeP st) {
		return WeakPtr<T>(st);
	}
public:
	inline SharedPtr<T> Lock() {
		return SharedPtr<T>(*this);
	}
public:
	inline operator Bool() const {
		return this->counter_ != 0 && this->counter_->strong != 0;
	}
private:
	Void _Create(const SharedPtr<T>& sp) {

		if (!&sp) {
			return;
		}

		this->reference_ = sp.reference_;
		this->counter_ = sp.counter_;

		if (this->counter_) {
			++this->counter_->weak;
		}
	}
	Void _Destroy() {

		if ((this->counter_ &&   this->counter_->weak == 0) ||
			(this->counter_ && --this->counter_->weak == 0 && this->counter_->strong == 0)) {

			delete this->counter_;
		}
	}
};

template <class T> class SharedPtrFromThis : public internal::__SharedPtrFromThis {
	friend class SharedPtr < T >;
public:
	SharedPtrFromThis() {
	}
protected:
	SharedPtr<T> GetSharedPtrFromThis() {
		return this->weakPtr.Lock();
	}
private:
	WeakPtr<T> weakPtr;
};

LAME_END2

#endif // ~__LAME_CORE_SHARED_PTR__