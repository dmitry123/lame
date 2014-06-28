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

#if 0
#  define printf __noop
#  define puts __noop
#endif

LAME_BEGIN2(Script)

using namespace Core;

typedef class ScriptError *ScriptErrorPtr, *const ScriptErrorPtrC;
typedef class ScriptLex *ScriptLexPtr, *const ScriptLexPtrC;
typedef class ScriptLexNode *ScriptLexNodePtr, *const ScriptLexNodePtrC;
typedef class ScriptParser *ScriptParserPtr, *const ScriptParserPtrC;
typedef class ScriptNode *ScriptNodePtr, *const ScriptNodePtrC;
typedef class ScriptBuilder *ScriptBuilderPtr, *const ScriptBuilderPtrC;
typedef class ScriptScope *ScriptScopePtr, *const ScriptScopePtrC;
typedef class ScriptObject *ScriptObjectPtr, *const ScriptObjectPtrC;
typedef class ScriptClass *ScriptClassPtr, *const ScriptClassPtrC;
typedef class ScriptAbstract *ScriptAbstractPtr, *const ScriptAbstractPtrC;
typedef class ScriptInterface *ScriptInterfacePtr, *const ScriptInterfacePtrC;
typedef class ScriptVar *ScriptVarPtr, *const ScriptVarPtrC;
typedef class ScriptMethod *ScriptMethodPtr, *const ScriptMethodPtrC;
typedef class ScriptScope *ScriptScopePtr, *const ScriptScopePtrC;
typedef class ScriptScopeController *ScriptScopeControllerPtr, *const ScriptScopeControllerPtrC;

typedef Buffer ScriptNativeString;
typedef Sint32 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Bool ScriptNativeBool;

typedef const Buffer& BufferRefC;

LAME_END2

#endif // ~__LAME_SCRIPT__DEFINE__