#ifndef __LAME_DYNAMIC_TYPES__
#define __LAME_DYNAMIC_TYPES__

#include "Define.h"

#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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

typedef va_list VaList;
typedef StringC LStringC;
typedef FILE* LFileHandle;
typedef String LString;

#endif // ~__LAME_DYNAMIC_TYPES__