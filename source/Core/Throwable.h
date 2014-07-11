#ifndef __LAME_CORE_THROWABLE__
#define __LAME_CORE_THROWABLE__

#include "Types.h"
#include "Stream.h"

LAME_BEGIN2(Core)

class LAME_API Throwable {
public:
	virtual Void Debug(PrintStreamPtr printer) = 0;
	virtual StringC What() = 0;
	virtual Void Throw() = 0;
protected:

};

LAME_END2

#endif // ~__LAME_CORE_THROWABLE__