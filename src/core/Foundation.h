#ifndef __LAME_CORE_FOUNDATION__
#define __LAME_CORE_FOUNDATION__

#include "Define.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

#if LAME_PLATFORM == LAME_PLATFORM_OSX
#  include <malloc/malloc.h>
#else
#  include <malloc.h>
#endif

#include <cassert>
#include <vector>
#include <deque>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <utility>
#include <string>

template <class T> using Vector = std::vector <T>;
template <class T> using Deque = std::deque <T>;
template <class T> using List = std::list <T>;
template <class T> using Stack = std::stack <T>;
template <class T> using Queue = std::queue <T>;
template <class T> using Set = std::set <T>;
template <class K, class T> using Map = std::map <K, T>;
template <class L, class R> using Pair = std::pair <L, R>;

typedef char SInt8, *SInt8P;
typedef unsigned char UInt8, *UInt8P;
typedef short SInt16, *SInt16P;
typedef unsigned short UInt16, *UInt16P;
typedef int SInt32, *SInt32P;
typedef unsigned int UInt32, *UInt32P;
typedef long long SInt64, *SInt64P;
typedef unsigned long long UInt64, *UInt64P;
typedef float Float32, *Float32P;
typedef double Float64, *Float64P;
typedef long double Float96, *Float96P;
typedef const char* StringPtrC;
typedef char* StringPtr;
typedef bool Logic, *LogicP;
typedef void Void, *VoidP;
typedef char Char, *CharP;

typedef std::string String;
typedef const std::string& ConstString;

#define strcasecmp stricmp

#endif // ~__LAME_CORE_FOUNDATION__