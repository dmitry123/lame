#ifndef __LAME_SCRIPT__CORE_SEGMENT__
#define __LAME_SCRIPT__CORE_SEGMENT__

#include "Define.h"
#include "Variable.h"

LAME_BEGIN2(Script)

class LAME_API Segment {
public:
	Segment(Uint32 size);
public:
	Void Write(VoidP block, Uint32 size);
	Void Write(VariablePtr var);
private:
	Uint32 offset;
	VoidP data;
	Uint32 size;
};

LAME_END2

#endif // ~__LAME_SCRIPT__CORE_SEGMENT__