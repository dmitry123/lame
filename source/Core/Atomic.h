#ifndef __LAME_CORE__ATOMIC__
#define __LAME_CORE__ATOMIC__

#include "define.h"
#include "types.h"

#if !defined(LAME_CPP)
typedef volatile Uint16 Atomic16, *Atomic16P;
typedef volatile Uint32 Atomic32, *Atomic32P;
typedef volatile Uint64 Atomic64, *Atomic64P;
#endif

#define __LAME_ATOMIC_CONSTRUCT(_bits) \
	public: Atomic##_bits(Uint##_bits counter = 0) { \
		this->counter_ = counter; \
	}

#define __LAME_ATOMIC_BASE(T) \
	public: \
		inline T operator ++ () { return this->Inc(); } \
		inline T operator ++ (int) { return this->Inc(); } \
		inline T operator -- () { return this->Dec(); } \
		inline T operator -- (int) { return this->Dec(); } \
		inline T operator = (T value) { return this->Set(value); } \
		inline T operator += (T value) { return this->Add(value); } \
		inline T operator -= (T value) { return this->Sub(value); } \
	public: \
		inline operator T () { return this->counter_; } \
	public: \
		volatile T counter_;

#define LAME_ATOMIC(_bits) \
	__LAME_ATOMIC_CONSTRUCT(_bits) \
	__LAME_ATOMIC_BASE(Uint##_bits)

LAME_BEGIN

class LAME_API Atomic16 {
	LAME_ATOMIC(16);
public:
	Uint16 Inc(Void);
	Uint16 Dec(Void);
	Uint16 Add(Uint16 value);
	Uint16 Sub(Uint16 value);
	Uint16 Set(Uint16 value);
};

class LAME_API Atomic32 {
	LAME_ATOMIC(32);
public:
	Uint32 Inc(Void);
	Uint32 Dec(Void);
	Uint32 Add(Uint32 value);
	Uint32 Sub(Uint32 value);
	Uint32 Set(Uint32 value);
};

class LAME_API Atomic64 {
	LAME_ATOMIC(64);
public:
	Uint64 Inc(Void);
	Uint64 Dec(Void);
	Uint64 Add(Uint64 value);
	Uint64 Sub(Uint64 value);
	Uint64 Set(Uint64 value);
};

LAME_END

#endif // ~__LAME_CORE__ATOMIC__