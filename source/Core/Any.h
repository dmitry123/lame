#ifndef __LAME_CORE_ANY__
#define __LAME_CORE_ANY__

#include "SharedPtr.h"

LAME_BEGIN2(Core)

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
	friend class internal::__AnySingleton < T >;
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

template <class T> using Arc = Any < T >;

LAME_END2

#endif // ~__LAME_CORE_ANY__