#ifndef __LAME_CORE_HASHABLE__
#define __LAME_CORE_HASHABLE__

#include "List.h"

LAME_BEGIN2(Core)

template <class T> class Hashable {
public:
	typedef T Type;
public:
	static inline Uint32 Size() {
		return sizeof(T);
	}
public:
	virtual T Hash() = 0;
};

class Hashable16 : public Hashable < Uint16 > {};
class Hashable32 : public Hashable < Uint32 > {};
class Hashable64 : public Hashable < Uint64 > {};

template <class T> class HashMap16 : public Map < Hashable16::Type, Hashable16 > {};
template <class T> class HashMap32 : public Map < Hashable32::Type, Hashable32 > {};
template <class T> class HashMap64 : public Map < Hashable64::Type, Hashable64 > {};

LAME_END2

#endif // ~__LAME_CORE_HASHABLE__