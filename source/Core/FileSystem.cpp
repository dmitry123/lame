#include "FileSystem.h"
#include "Utility.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef LAME_WINDOWS
#  include <unistd.h>
#else
#  include <Windows.h>
#endif

#include <sys/stat.h>
#include <fstream>

#ifdef LAME_WINDOWS
#  undef CreateDirectory
#  undef RemoveDirectory
#  undef GetCurrentDirectory
#  undef CreateFile
#  undef MoveFile
#  undef CopyFile
#endif

#ifdef LAME_WINDOWS
#  define strcasecmp _stricmp
#endif

typedef struct stat Stat;

LAME_BEGIN



LAME_END