#ifndef __LAME_CORE_SIZEABLE__
#define __LAME_CORE_SIZEABLE__

#include "Types.h"

LAME_BEGIN2(Core)

class Sizeable {
public:
	virtual Uint32 GetSize() const = 0;
public:
	virtual Uint64 GetSize64() const {
		return Uint64(this->GetSize());
	}
};

LAME_END2

#endif // ~__LAME_CORE_SIZEABLE__