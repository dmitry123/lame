#include "Timer.h"

#include <time.h>
#include <stdio.h>

#ifdef LAME_WINDOWS
#  include <Windows.h>
#else
#  include <unistd.h>
#endif

typedef time_t TIMET, *TIMETP;
typedef struct tm TM, *TMP;

LAME_BEGIN

Buffer Time::ConvertToString(Void) {

	Buffer buffer;

	buffer.resize(30);
	buffer.Format("%d.%d.%d %d:%d:%d",
		(Sint32)this->year,
		(Sint32)this->month,
		(Sint32)this->day,
		(Sint32)this->hour,
		(Sint32)this->minute,
		(Sint32)this->second
		);

	return buffer;
}

Uint64 Time::ConvertToMilliseconds(Void) {

	return
		this->year * LAME_MS_YEAR +
		this->month * LAME_MS_MONTH +
		this->day * LAME_MS_DAY +
		this->hour * LAME_MS_HOUR +
		this->minute * LAME_MS_MINUTE +
		this->second * LAME_MS_SECOND +
		this->msecond;
}

Void Time::BuildForDate(Clock milliseconds) {

	TIMET current;
	TMP today;

	current = (TIMET)milliseconds;
	today = localtime(&current);

	this->day = today->tm_mday;
	this->hour = today->tm_hour;
	this->minute = today->tm_min;
	this->month = today->tm_mon + 1;
	this->year = today->tm_year + 1900;
	this->second = today->tm_sec;
}

Void Time::BuildTorTime(Clock milliseconds) {

	this->year = Uint32(milliseconds / LAME_MS_YEAR);
	milliseconds %= LAME_MS_YEAR;
	this->month = Uint32(milliseconds / LAME_MS_MONTH);
	milliseconds %= LAME_MS_MONTH;
	this->day = Uint32(milliseconds / LAME_MS_DAY);
	milliseconds %= LAME_MS_DAY;
	this->hour = Uint32(milliseconds / LAME_MS_HOUR);
	milliseconds %= LAME_MS_HOUR;
	this->minute = Uint32(milliseconds / LAME_MS_MINUTE);
	milliseconds %= LAME_MS_MINUTE;
	this->second = Uint32(milliseconds / LAME_MS_SECOND);
	milliseconds %= LAME_MS_SECOND;
	this->msecond = Uint32(milliseconds);
}

Void Time::BuildForCurrentDate(Void) {

	TIMET t;
	this->BuildForDate(time(&t));
}

Clock Time::GetTime(Void) {
#ifdef LAME_WINDOWS
	return clock();
#else
    return clock() / 1000;
#endif
}

Void Time::Sleep(Clock milliseconds) {

#ifdef LAME_WINDOWS
	::Sleep((DWORD)milliseconds);
#else
	::usleep((useconds_t)milliseconds * 1000);
#endif
}

Time Time::GetCurrentDate(Void) {

	Time t;
	t.BuildForCurrentDate();
	return t;
}

LAME_END