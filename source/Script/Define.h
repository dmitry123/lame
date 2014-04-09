#ifndef __LAME_SCRIPT__DEFINE__
#define __LAME_SCRIPT__DEFINE__

#include <Core/Core.h>

#if defined(LAME_API)
#  undef LAME_API
#endif

#if defined(LAME_MSVC)
#  if defined(SCRIPT_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

LAME_BEGIN

typedef Sint64 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Buffer ScriptNativeString;
typedef Bool ScriptNativeBool;

LAME_END

#endif // ~__LAME_SCRIPT__DEFINE__