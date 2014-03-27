#ifndef __LAME_CORE__TIMER__
#define __LAME_CORE__TIMER__

#include "Define.h"
#include "Types.h"
#include "Utility.h"
#include "Event.h"

LAME_BEGIN

#if defined(_TIME_T_DEFINED)
typedef time_t Clock;
#else
#  if defined(DK_X64)
typedef long Clock;
#  else
typedef long long Clock;
#  endif
#endif

typedef class Time Time, *TimePtr;
typedef class Timer Timer, *TimerPtr;

class LAME_API Time {
public:
	Buffer ConvertToString(Void);
	Uint64 ConvertToMilliseconds(Void);
public:
	Void BuildForDate(Clock milliseconds);
	Void BuildTorTime(Clock milliseconds);
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

class LAME_API Timer {
public:
	typedef Event <Void(TimerPtr)> Callback;
public:
	Void Create(Callback callback, Clock interval);
	Bool Update(Void);
	Void Terminate(Void);
	Void Suspend(Void);
	Void Resume(Void);
	Void Reset(Void);
public:
	inline Callback GetCallback() const {
		return this->callback_;
	}
	inline Clock GetInterval() const {
		return this->interval_;
	}
	inline Void SetInterval(Clock interval) {
		this->interval_ = interval;
	}
private:
	Callback callback_;
	Clock interval_ = 0;
	Clock last_ = 0;
	Bool suspended_ = 0;
};

LAME_END

#endif // ~__LAME_CORE__TIMER__