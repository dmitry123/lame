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

typedef class ScriptLex *ScriptLexPtr, *const ScriptLexPtrC;
typedef class ScriptLexNode *ScriptLexNodePtr, *const ScriptLexNodePtrC;
typedef class ScriptParser *ScriptParserPtr, *const ScriptParserPtrC;
typedef class ScriptNode *ScriptNodePtr, *const ScriptNodePtrC;
typedef class ScriptBuilder *ScriptBuilderPtr, *const ScriptBuilderPtrC;
typedef class ScriptVar *ScriptVarPtr, *const ScriptVarPtrC;
typedef class ScriptManager *ScriptManagerPtr, *const ScriptManagerPtrC;
typedef class ScriptPerformer *ScriptPerformerPtr, *const ScriptPerformerPtrC;

typedef Sint64 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Buffer ScriptNativeString;
typedef Bool ScriptNativeBool;
typedef SharedPtr<ScriptNode> ScriptNativeClass;

typedef Void(*ScriptCallback)(ScriptNodePtr var, VoidP data);
typedef const Buffer& BufferRefC;

LAME_END

#endif // ~__LAME_SCRIPT__DEFINE__