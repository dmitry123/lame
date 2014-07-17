#include "Time.h"
#include "Buffer.h"

#include <time.h>
#include <stdio.h>

#ifdef LAME_WINDOWS
#  include <Windows.h>
#else
#  include <unistd.h>
#endif

typedef time_t TIMET, *TIMETP;
typedef struct tm TM, *TMP;

LAME_BEGIN2(Core)

Void Time::ConvertToString(String buffer) {

	Buffer b;

	b.resize(30);
	b.Format("%d.%d.%d %d:%d:%d",
		(Sint32)this->year,
		(Sint32)this->month,
		(Sint32)this->day,
		(Sint32)this->hour,
		(Sint32)this->minute,
		(Sint32)this->second
	);

	strcpy(buffer, b.data());
}

Uint64 Time::ConvertToMilliseconds(Void) {

	return
		this->year * LAME_TIME_YEAR +
		this->month * LAME_TIME_MONTH +
		this->day * LAME_TIME_DAY +
		this->hour * LAME_TIME_HOUR +
		this->minute * LAME_TIME_MINUTE +
		this->second * LAME_TIME_SECOND +
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

Void Time::BuildForTime(Clock milliseconds) {

	this->year = Uint32(milliseconds / LAME_TIME_YEAR);
	milliseconds %= LAME_TIME_YEAR;
	this->month = Uint32(milliseconds / LAME_TIME_MONTH);
	milliseconds %= LAME_TIME_MONTH;
	this->day = Uint32(milliseconds / LAME_TIME_DAY);
	milliseconds %= LAME_TIME_DAY;
	this->hour = Uint32(milliseconds / LAME_TIME_HOUR);
	milliseconds %= LAME_TIME_HOUR;
	this->minute = Uint32(milliseconds / LAME_TIME_MINUTE);
	milliseconds %= LAME_TIME_MINUTE;
	this->second = Uint32(milliseconds / LAME_TIME_SECOND);
	milliseconds %= LAME_TIME_SECOND;
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

LAME_END2
