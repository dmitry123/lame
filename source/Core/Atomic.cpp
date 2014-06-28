#include "Atomic.h"

#if defined(LAME_WINDOWS)
#  include <Windows.h>
#elif defined(LAME_APPLE)
#  include <libkern/OSAtomic.h>
#endif

LAME_BEGIN2(Core)

/* 16 bits */

Uint16 Atomic16::Inc(Void) {
    
#ifdef LAME_WINDOWS
	InterlockedIncrement16((PSHORT)&this->counter_);
#else
    OSAtomicIncrement32((volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint16 Atomic16::Dec(Void) {

#ifdef LAME_WINDOWS
	InterlockedDecrement16((PSHORT)&this->counter_);
#else
    OSAtomicDecrement32((volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint16 Atomic16::Add(Uint16 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd((PLONG)&this->counter_, (LONG)value);
#else
    OSAtomicAdd32(value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint16 Atomic16::Sub(Uint16 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd((PLONG)&this->counter_, -(LONG)value);
#else
    OSAtomicAdd32(-(Sint16)value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint16 Atomic16::Set(Uint16 value) {

#ifdef LAME_WINDOWS
	InterlockedCompareExchange16((PSHORT)&this->counter_, value, this->counter_);
#else
    OSAtomicCompareAndSwap32(this->counter_, value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

/* 32 bits */

Uint32 Atomic32::Inc(Void) {
    
#ifdef LAME_WINDOWS
	InterlockedIncrement((PLONG)&this->counter_);
#else
    OSAtomicIncrement32((volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint32 Atomic32::Dec(Void) {
    
#ifdef LAME_WINDOWS
	InterlockedDecrement((PLONG)&this->counter_);
#else
    OSAtomicDecrement32((volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint32 Atomic32::Add(Uint32 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd((PLONG)&this->counter_, (LONG)value);
#else
    OSAtomicAdd32(value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint32 Atomic32::Sub(Uint32 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd((PLONG)&this->counter_, -(LONG)value);
#else
    OSAtomicAdd32(-(Sint32)value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint32 Atomic32::Set(Uint32 value) {

#ifdef LAME_WINDOWS
	InterlockedCompareExchange((PLONG)&this->counter_, value, this->counter_);
#else
    OSAtomicCompareAndSwap32(this->counter_, value, (volatile int32_t*)&this->counter_);
#endif
    
    return this->counter_;
}

/* 64 bits */

Uint64 Atomic64::Inc(Void) {
    
#ifdef LAME_WINDOWS
	InterlockedIncrement64((PLONGLONG)&this->counter_);
#else
    OSAtomicIncrement64((volatile int64_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint64 Atomic64::Dec(Void) {
    
#ifdef LAME_WINDOWS
	InterlockedDecrement64((PLONGLONG)&this->counter_);
#else
    OSAtomicDecrement64((volatile int64_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint64 Atomic64::Add(Uint64 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd64((PLONGLONG)&this->counter_, (LONGLONG)value);
#else
    OSAtomicAdd64(value, (volatile int64_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint64 Atomic64::Sub(Uint64 value) {
    
#ifdef LAME_WINDOWS
	InterlockedAdd64((PLONGLONG)&this->counter_, -(LONGLONG)value);
#else
    OSAtomicAdd64(-(Sint64)value, (volatile int64_t*)&this->counter_);
#endif
    
    return this->counter_;
}

Uint64 Atomic64::Set(Uint64 value) {
    
#ifdef LAME_WINDOWS
	InterlockedCompareExchange64((PLONGLONG)&this->counter_, value, this->counter_);
#else
    OSAtomicCompareAndSwap64(this->counter_, value, (volatile int64_t*)&this->counter_);
#endif
    
    return this->counter_;
}

LAME_END2
