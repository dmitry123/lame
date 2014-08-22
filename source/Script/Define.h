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

LAME_BEGIN2(Script)

using namespace Core;

typedef class Lex *LexPtr, *const LexPtrC;
typedef class LexNode *LexNodePtr, *const LexNodePtrC;
typedef class FileParser *FileParserPtr, *const FileParserPtrC;
typedef class Node *NodePtr, *const NodePtrC;
typedef class NodeBuilder *NodeBuilderPtr, *const NodeBuilderPtrC;
typedef class Scope *ScopePtr, *const ScopePtrC;
typedef class Object *ObjectPtr, *const ObjectPtrC;
typedef class Class *ClassPtr, *const ClassPtrC;
typedef class Interface *InterfacePtr, *const InterfacePtrC;
typedef class Variable *VariablePtr, *const VariablePtrC;
typedef class Method *MethodPtr, *const MethodPtrC;
typedef class Scope *ScopePtr, *const ScopePtrC;
typedef class ScopeBuilder *ScopeBuilderPtr, *const ScopeBuilderPtrC;
typedef class Segment *SegmentPtr, *const SegmentPtrC;

typedef Buffer ScriptNativeString;
typedef Sint64 ScriptNativeInt;
typedef Float64 ScriptNativeFloat;
typedef Bool ScriptNativeBool;

typedef Exception SegmentLinkerException;
typedef Exception SegmentException;

LAME_END2

#endif // ~__LAME_SCRIPT__DEFINE__