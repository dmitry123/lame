#ifndef __LAME_DYNAMIC_DOS_READER__
#define __LAME_DYNAMIC_DOS_READER__

#include "Types.h"

/*
	DLL File Structure {

		1. DOS-Header + DOS-Stub

		2. PE-Header

		3. Section-Header {
			Section-1
			Section-2
			...
			Section-N
		}
	}

	The DOS header is only used for backwards compatibility. It precedes
	the DOS stub that normally just displays an error message about the
	program not being able to be run from DOS mode.

	The PE header contains informations about the different sections inside
	the executable that are used to store code and data or to define imports
	from other libraries or exports this libraries provides.
*/

#include <Windows.h>

typedef VoidP LMemoryModule;
typedef VoidP LMemoryResource;
typedef VoidP LModule;

#ifdef __cplusplus
extern "C" {
#endif

typedef Sint32(*LFarProc)(Void);
typedef LModule(*LLoadLibraryProc)(LStringC, VoidP);
typedef Void(*LFreeLibraryProc)(LModule, VoidP);
typedef LFarProc(*LGetProcAddressProc)(LModule, LStringC, VoidP);

LModule LAME_API LLoadLibrary(LStringC);
LModule LAME_API LLoadLibraryEx(LStringC,
	LLoadLibraryProc,
	LFreeLibraryProc,
	LGetProcAddressProc);

LFarProc LAME_API LGetProcAddress(LModule, LStringC);
Void LAME_API LFreeLibrary(LModule);

LMemoryResource LAME_API LFindResource(LModule, LStringC, LStringC);
LMemoryResource LAME_API LFindResourceEx(LModule, LStringC, LStringC, Uint32);

Uint32 LAME_API LSizeOfResource(LModule, LMemoryResource);
VoidP LAME_API LLoadResource(LModule, LMemoryResource);

Uint32 LAME_API LLoadString(LMemoryModule, Uint32, LStringC, Sint32);
Uint32 LAME_API LLoadStringEx(LMemoryModule, Uint32, LStringC, Sint32, Uint32);

#ifdef LAME_CPP
}
#endif

#endif // ~__LAME_DYNAMIC_DOS_READER__