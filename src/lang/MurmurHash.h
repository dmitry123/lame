#ifndef __LAME_LANG__MURMUR_HASH
#define __LAME_LANG__MURMUR_HASH

#include "Foundation.h"

LAME_LANG_BEGIN

class MurmurHash {
	static const UInt32 SEED = 0x7b1010;
public:
	static UInt32 calculate32(StringPtrC string, UInt32 seed);
	static UInt64 calculate64(StringPtrC string, UInt32 seed);
public:
	static inline UInt32 calculate32(StringPtrC string) {
		return calculate32(string, SEED);
	}
	static inline UInt64 calculate64(StringPtrC string) {
		return calculate64(string, SEED);
	}
};

LAME_LANG_END

#endif // ~__LAME_LANG__MURMUR_HASH