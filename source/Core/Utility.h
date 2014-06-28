#ifndef __LAME_CORE__UTILITY__
#define __LAME_CORE__UTILITY__

#include "Define.h"
#include "Types.h"
#include "Atomic.h"
#include "List.h"

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

#define LAME_SHAREDPTR

#include <memory>

LAME_BEGIN2(Core)

inline VoidP Malloc(Uint32 size) {
	return ::malloc(size);
}

inline Void Free(VoidP pointer) {
	free(pointer);
}

inline VoidP Realloc(VoidP pointer, Uint32 size) {
	return realloc(pointer, size);
}

inline VoidP ZeroMemory(VoidP pointer, Uint32 size) {
	return memset(pointer, 0, size);
}

inline VoidP ZeroAlloc(Uint32 size) {
	return ZeroMemory(Malloc(size), size);
}

#if !defined(LAME_SHAREDPTR)

template <class T> using SharedPtr = std::shared_ptr<T>;
template <class T> using WeakPtr = std::weak_ptr<T>;

#else

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
		this->__Destroy();
	}
public:
	typedef T Type, *TypeP, &TypeR;
public:
	inline explicit SharedPtr(TypeP reference) : SharedPtr() {
		this->__Create(reference);
	}
	inline SharedPtr(const SharedPtr <T>& sp) : SharedPtr() {
		this->__Copy(sp);
	}
public:
	inline Void operator = (TypeP reference) {
		this->__Create(reference);
	}
	inline Void operator = (const SharedPtr <T>& sp) {
		this->__Copy(sp);
	}
	inline Void operator () (TypeP reference) {
		this->__Create(reference);
	}
protected:
	Void __Create(TypeP reference);
	Void __Destroy();
	Void __Copy(const SharedPtr <T>& sp);
public:
	Void UnLink();
public:
	inline Void reset() {
		this->UnLink();
	}
	inline Void reset(TypeP reference) {
		this->__Create(reference);
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
		((SharedPtrFromThis<T>*) ptr)->weakPtr.__Create(*this);
	}
public:
	TypeP reference_;
	StrongWeakCounterPtr counter_;
};

template <class T>
Void SharedPtr <T>::__Create(TypeP reference) {

	if (!reference || !this) {
		return;
	}

	if (this->counter_) {
		this->__Destroy();
	}

	this->reference_ = reference;
	this->counter_ = new StrongWeakCounter();
	this->counter_->strong = 1;
	this->counter_->weak = 0;

	this->_EnableSharedPtr(this->reference_);
}

template <class T>
Void SharedPtr <T>::__Destroy() {

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
Void SharedPtr <T>::__Copy(const SharedPtr <T>& sp) {

	if (!&sp || !this) {
		return;
	}

	if (this->counter_) {
		this->__Destroy();
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
	friend class SharedPtr < T > ;
public:
	WeakPtr() {
	}
	~WeakPtr() {
		this->UnLink();
	}
public:
	explicit WeakPtr(const SharedPtr<T>& sp) {
		this->__Create(sp);
	}
	explicit WeakPtr(typename SharedPtr<T>::TypeP st) {
		this->__Create(st);
	}
public:
	inline Void reset() {
		this->UnLink();
	}
public:
	inline Void UnLink() {
		this->__Destroy();
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
	Void __Create(const SharedPtr<T>& sp) {

		if (!&sp) {
			return;
		}

		this->reference_ = sp.reference_;
		this->counter_ = sp.counter_;

		if (this->counter_) {
			++this->counter_->weak;
		}
	}
	Void __Destroy() {

		if ((this->counter_ &&   this->counter_->weak == 0) ||
			(this->counter_ && --this->counter_->weak == 0 && this->counter_->strong == 0)) {

			delete this->counter_;
		}
	}
};

template <class T> class SharedPtrFromThis : public internal::__SharedPtrFromThis {
	friend class SharedPtr < T > ;
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

#endif

class Buffer : public std::string {
public:
	Buffer() {
	}
	~Buffer() {
	}
public:
	Buffer(const char* string) : std::string(string) {}
	Buffer(char* string) : std::string(string) {}
	Buffer(const char  symbol) : std::string(&symbol, 1) {}
	Buffer(const std::string& string) : std::string(string) {}
	Buffer(const char* string, Uint32 length) : std::string(string, length) {}
	Buffer(char* string, Uint32 length) : std::string(string, length) {}
	Buffer(const std::string& string, Uint32 length) : std::string(string, length) {}
public:
	Void LAME_API Format(StringC format, ...);
	List <Buffer> LAME_API Split(Sint8 symbol) const;
	Void LAME_API Get(Void);
	Uint32 LAME_API GetHash(Void) const;
public:
	static Uint32 GetHash(StringC string) {
		return Buffer(string).GetHash();
	}
};

namespace internal {
	template <class T> class __AnySingleton {
	public:
		typedef T Type, *TypePtr, &TypeRef;
	public:
		inline Bool operator == (const __AnySingleton<T>& base) const {
			return this->instance_.reference_ == base.instance_.reference_;
		}
		inline Bool operator != (const __AnySingleton<T>& base) const {
			return this->instance_.reference_ != base.instance_.reference_;
		}
	public:
		inline SharedPtr<T> GetSharedPtr() {
			return this->instance_;
		}
		inline WeakPtr<T> GetWeakPtr() {
			return WeakPtr<T>(instance_);
		}
	public:
		inline TypeRef operator * () {
			return *this->instance_.reference_;
		}
		inline TypePtr operator -> () {
			return this->instance_.reference_;
		}
	public:
		inline operator Bool () const {
			return this->instance_.reference_ != 0;
		}
	public:
		SharedPtr<T> instance_;
	};
}

template <class T> class Any : public internal::__AnySingleton < T > {
	friend class internal::__AnySingleton < T > ;
public:
	explicit Any(Bool construct = LAME_FALSE) {
		if (construct && !this->instance_) {
			this->__Construct();
		}
	}
	~Any() {
		this->instance_.UnLink();
	}
public:
	Any(const Any<T>& any) {
		if (!any.instance_) {
			this->__Construct();
		}
		this->instance_ = any.instance_;
	}
	Any(const internal::__AnySingleton<T>& any) {
		if (!any.instance_) {
			this->__Construct();
		}
		this->instance_ = any.instance_;
	}
public:
	inline typename internal::__AnySingleton<T>::TypeRef operator * () {
		if (!this->instance_) {
			this->__Construct();
		}
		return *this->instance_.reference_;
	}
	inline typename internal::__AnySingleton<T>::TypePtr operator -> () {
		if (!this->instance_) {
			this->__Construct();
		}
		return this->instance_.reference_;
	}
public:
	inline Any<T>& operator = (const Any<T>& any) {
		if (!any.instance_) {
			((Any<T>*)&any)->__Construct();
		}
		this->instance_ = any.instance_;
		return *this;
	}
public:
	inline Any<T> Clone() {
		Any<T> any;
		*any = *this;
		return any;
	}
private:
	Void __Construct(Void) {
		this->instance_(new typename internal::__AnySingleton<T>::Type);
	}
};

template <class T> using Arc = Any < T > ;

class LAME_API Exception {
public:
	Exception(Void) {
		ZeroMemory(this->buffer_, sizeof(this->buffer_));
	}
public:
	Exception(
		StringC file,
		Uint32 line,
		StringC function,
		StringC message, ...);
public:
	inline StringC What() {
		return this->buffer_;
	}
	inline Void Throw() {
		throw this;
	}
	inline Void Debug() {
		printf("%s\n", this->buffer_);
	}
protected:
	ErrorBuffer buffer_;
};

#define PostErrorMessage(_message, ...) \
	throw ::LAME_NAMESPACE::Core::Exception(__FILE__, __LINE__, __FUNCTION__, _message, __VA_ARGS__);

#define PostWarningMessage(_message, ...) \
	throw ::LAME_NAMESPACE::Core::Exception(__FILE__, __LINE__, __FUNCTION__, _message, __VA_ARGS__);

class LAME_API Zlib {
public:
	Bool Compress(VoidP buffer, Uint32 length);
	Bool UnCompress(VoidP buffer, Uint32 length);
public:
	Zlib();
	~Zlib();
public:
	VoidP buffer;
	Uint32 length;
};

LAME_END2

#endif // ~__LAME_CORE__UTILITY__