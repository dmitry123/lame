#ifndef __LAME_CORE_TIME__
#define __LAME_CORE_TIME__

#include "Types.h"

LAME_BEGIN2(Core)

class LAME_API Time {
public:
	Time() {
		// default
	}
public:
	Void ConvertToString(String buffer);
	Uint64 ConvertToMilliseconds(Void);
public:
	Void BuildForDate(Clock milliseconds);
	Void BuildForTime(Clock milliseconds);
	Void BuildForCurrentDate(Void);
public:
	static Clock GetTime(Void);
	static Void Sleep(Clock milliseconds);
	static Time GetCurrentDate(Void);
public:
	Uint32 year = 0;
	Uint32 month = 0;
	Uint32 day = 0;
	Uint32 hour = 0;
	Uint32 minute = 0;
	Uint32 second = 0;
	Uint32 msecond = 0;
};

LAME_END2

#endif // ~__LAME_CORE_TIME__