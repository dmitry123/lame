#ifndef __LAME_CORE__UTILITY__
#define __LAME_CORE__UTILITY__

#include "define.h"
#include "types.h"
#include "atomic.h"
#include "list.h"

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

LAME_BEGIN

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
}

template <class T>
Void SharedPtr <T>::__Destroy() {
    
	if (!this || (this->counter_ && this->counter_->weak != 0)) {
		return;
	}
    
	if ((this->counter_ &&   this->counter_->strong == 0) ||
		(this->counter_ && --this->counter_->strong == 0)) {
        
        delete this->reference_;
		delete this->counter_;
        
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

template <class T> class WeakPtr : public SharedPtr<T> {
public:
    WeakPtr() {
    }
	~WeakPtr() {
		this->UnLink();
        
		if (this->counter_) {
			this->counter_->strong.Inc();
		}
	}
public:
    explicit WeakPtr(const SharedPtr<T>& sp) {
		this->__Create(sp);
    }
	explicit WeakPtr(typename SharedPtr<T>::TypeP st) {
		this->__Create(st);
	}
public:
    inline T& operator * () {
		return *this->reference_;
    }
    inline T* operator-> () {
		return this->reference_;
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
        
		SharedPtr<T> sp;
        
		sp.reference_ = this->reference_;
		sp.counter_ = this->counter_;
        
		return sp;
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
	Void __Create(typename SharedPtr<T>::TypeP st) {
        
		this->reference_ = st;
	}
	Void __Destroy() {
        
		if ((this->counter_ &&   this->counter_->weak == 0) ||
			(this->counter_ && --this->counter_->weak == 0 && this->counter_->strong == 0)) {
            
			this->SharedPtr<T>::__Destroy();
		}
	}
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
	Buffer(      char* string) : std::string(string) {}
    Buffer(const char  symbol) : std::string(&symbol, 1) {}
	Buffer(const std::string& string) : std::string(string) {}
	Buffer(const char* string, Uint32 length) : std::string(string, length) {}
	Buffer(      char* string, Uint32 length) : std::string(string, length) {}
	Buffer(const std::string& string, Uint32 length) : std::string(string, length) {}
public:
	Void Format(StringC format, ...) {
        
		int size = 512;
		char* buffer = new char[size];
        
		va_list vl;
		va_start(vl, format);
        
		int nsize = vsnprintf(buffer, size, format, vl);
        
		if (size <= nsize) {
            
			delete[] buffer;
            
			buffer = 0;
			buffer = new char[nsize + 1];
            
			vsnprintf(buffer, size, format, vl);
		}
        
		*this = buffer;
        
		va_end(vl);
        
		delete[] buffer;
	}
    
	List <Buffer> Split(Sint8 symbol) {
        
		List <Buffer> list;
		StringC first;
		StringC where;
		Uint32 last;
		Sint8P buffer;
        
		buffer = (Sint8P)this->data();
		first = this->data();
		last = 0;
        
		while ((first = strchr(first + 1, symbol))) {
            
			buffer[first - buffer] = '\0';
            
			where = buffer + last;
            
			while (*where == symbol) {
				++where;
			}
            
			if (*where) {
				list.push_back(where);
			}
            
			last += list.back().length();
            
			buffer[first - buffer] = symbol;
            
			while (*first == symbol) {
				++first;
				++last;
			}
		}
        
		where = buffer + last;
        
		while (*where == symbol) {
			++where;
		}
        
		if (*where) {
			list.push_back(where);
		}
        
		return list;
	}
    
	Void Get(Void) {
        
		Sint32 symbol;
		Buffer string;
        
		while (LAME_TRUE) {
            
			symbol = getchar();
            
			if (symbol == '\n' || symbol == '\r') {
				break;
			}
            
			if (symbol == '\b' && string.length() > 0) {
				string.pop_back();
			}
            
			string += symbol;
		}
        
		*this = string;
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

template <class T> class Any : public internal::__AnySingleton<T> {
	friend class internal::__AnySingleton<T>;
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

template <class T> using Arc = Any<T>;

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
	throw LAME_NAMESPACE::Exception(__FILE__, __LINE__, __FUNCTION__, _message, __VA_ARGS__);

#define PostWarningMessage(_message, ...) \
	throw LAME_NAMESPACE::Exception(__FILE__, __LINE__, __FUNCTION__, _message, __VA_ARGS__);

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

LAME_END

#endif // ~__LAME_CORE__UTILITY__