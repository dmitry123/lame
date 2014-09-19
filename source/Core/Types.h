#ifndef __LAME_CORE__TYPES__
#define __LAME_CORE__TYPES__

#include "Define.h"

#include <string.h>
#include <memory.h>
#include <stdarg.h>

LAME_BEGIN

typedef char Sint8, *Sint8P;
typedef unsigned char Uint8, *Uint8P;
typedef short Sint16, *Sint16P;
typedef unsigned short Uint16, *Uint16P;
typedef int Sint32, *Sint32P;
typedef unsigned int Uint32, *Uint32P;
typedef long long Sint64, *Sint64P;
typedef unsigned long long Uint64, *Uint64P;
typedef float Float32, *Float32P;
typedef double Float64, *Float64P;
typedef long double Float96, *Float96P;
typedef const char* StringC;
typedef char* String;
typedef unsigned char Bool, *BoolP;
typedef void Void, *VoidP;

#if defined(LAME_CLANG)
#  define Void void
#endif

typedef char FileSignatureBuffer[LAME_SIGNATURE_MAX];
typedef char FileNameBuffer[LAME_FILENAME_MAX];
typedef char FilePathBuffer[LAME_FILEPATH_MAX];
typedef char FileExtensionBuffer[LAME_FILEEXTENSION_MAX];
typedef char ErrorBuffer[LAME_ERRORBUFFER_MAX];
typedef char TimeBuffer[LAME_TIMEBUFFER_MAX];
typedef char FileSignature[LAME_SIGNATURE_MAX];
typedef char FileName[LAME_FILENAME_MAX];
typedef char FilePath[LAME_FILEPATH_MAX];
typedef char FileExtension[LAME_FILEEXTENSION_MAX];

#if defined(_TIME_T_DEFINED)
typedef time_t Clock;
#else
#  if defined(LAME_X64)
typedef long Clock;
#  else
typedef long long Clock;
#  endif
#endif

#if !defined(LAME_CPP)
typedef Uint16 Flag16, *Flag16P;
typedef Uint32 Flag32, *Flag32P;
typedef Uint64 Flag64, *Flag64P;
#endif

#if !defined(LAME_CPP)
typedef struct TimeInfo TimeInfo, *TimeInfoP;
typedef struct FindFileInfo FindFileInfo, *FindFileInfoP;

struct TimeInfo {
	Uint32 year;
	Uint32 month;
	Uint32 day;
	Uint32 hour;
	Uint32 minute;
	Uint32 second;
	Uint32 millisecond;
};

struct FindFileInfo {
	FilePath path;
	Uint32 size;
	FindFileInfoP next;
};
#endif

typedef va_list VaList;

typedef struct {
	Sint32 x;
	Sint32 y;
} Point, *PointPtr;

LAME_END

#define LAME_CLASS(_name) \
	typedef class _name *_name##Ptr, *const _name##PtrC

LAME_BEGIN2(Core)
LAME_CLASS(Stream);
LAME_CLASS(InputStream);
LAME_CLASS(OutputStream);
LAME_CLASS(PrintStream);
LAME_CLASS(ConsolePrinter);
LAME_CLASS(Condition);
LAME_CLASS(Locker);
LAME_CLASS(Mutex);
LAME_CLASS(Thread);
LAME_CLASS(Timer);
LAME_CLASS(WaitManager);
LAME_CLASS(Console);
LAME_CLASS(Hashable16);
LAME_CLASS(Hashable32);
LAME_CLASS(Hashable64);
LAME_CLASS(Library);
LAME_END2

#endif // ~__LAME_CORE__TYPES__