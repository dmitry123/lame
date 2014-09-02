#ifndef __LAME_CORE_EXCEPTION__
#define __LAME_CORE_EXCEPTION__

#include "ThrowableAdapter.h"

LAME_BEGIN2(Core)
	
class Exception : public ThrowableAdapter {
public:
	LAME_API Exception(StringC message, ...);
};

typedef Exception MutexException;
typedef Exception WaitManagerException;
typedef Exception FileException;
typedef Exception DirectoryException;

#define LAME_ASSERT(_condition) \
    if (!(_condition)) { \
        throw Exception(#_condition); \
    }

LAME_END2

#endif // ~__LAME_CORE_EXCEPTION__