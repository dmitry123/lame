#ifndef __LAME_VIRTUAL_MACHINE__DEFINE__
#define __LAME_VIRTUAL_MACHINE__DEFINE__

#include <Core/Core.h>

#undef LAME_API

#if defined(LAME_MSVC)
#  if defined(VIRTUALMACHINE_EXPORTS)
#    define LAME_API __declspec(dllexport)
#  else
#    define LAME_API __declspec(dllimport)
#  endif
#else
#  define LAME_API
#endif

LAME_BEGIN2(VirtualMachine)

using namespace ::Lame::Core;

typedef class ByteCodePrinter *ByteCodePrinterPtr, *const ByteCodePrinterPtrC;

LAME_END2

#endif // ~__LAME_VIRTUAL_MACHINE__DEFINE__