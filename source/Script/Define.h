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
typedef class Abstract *AbstractPtr, *const AbstractPtrC;
typedef class Array *ArrayPtr, *const ArrayPtrC;
typedef class Class *ClassPtr, *const ClassPtrC;
typedef class Error *ErrorPtr, *const ErrorPtrC;
typedef class GlobalScope *GlobalScopePtr, *const GlobalScopePtrC;
typedef class Interface *InterfacePtr, *const InterfacePtrC;
typedef class Method *MethodPtr, *const MethodPtrC;
typedef class Object *ObjectPtr, *const ObjectPtrC;
typedef class Scope *ScopePtr, *const ScopePtrC;
typedef class ScopeController *ScopeControllerPtr, *const ScopeControllerPtrC;
typedef class Variable *VariablePtr, *const VariablePtrC;
typedef class Segment *SegmentPtr, *const SegmentPtrC;
typedef class SegmentBuilder *SegmentSegmentBuilderPtr, *const SegmentSegmentBuilderPtrC;
typedef class SegmentLinker *SegmentLinkerPtr, *const SegmentLinkerPtrC;

typedef Buffer ScriptNativeString;
typedef Sint64 ScriptNativeInt;
typedef Float64 ScriptNativeFloat;
typedef Bool ScriptNativeBool;

typedef Exception SegmentLinkerException;
typedef Exception SegmentException;

LAME_END2

#endif // ~__LAME_SCRIPT__DEFINE__