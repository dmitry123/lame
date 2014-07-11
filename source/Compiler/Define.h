#ifndef __LAME_SCRIPT_COMPILER_DEFINE__
#define __LAME_SCRIPT_COMPILER_DEFINE__

#include <Script/Script.h>

#if defined(LAME_API)
#  undef LAME_API
#endif

#if defined(LAME_MSVC)
#  if defined(COMPILER_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

LAME_BEGIN2(Compiler)

typedef class CodeTranslator *CodeTranslatorPtr, *const CodeTranslatorPtrC;
typedef class CodeAnalizer *CodeAnalizerPtr, *const CodeAnalizerPtrC;

LAME_END2

#endif // ~__LAME_SCRIPT_COMPILER_DEFINE__