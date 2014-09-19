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

typedef class Class *ClassPtr, *const ClassPtrC;
typedef class CodeBuilder *CodeBuilderPtr, *const CodeBuilderPtrC;
typedef class CodeNode *CodeNodePtr, *const CodeNodePtrC;
typedef class FileParser *FileParserPtr, *const FileParserPtrC;
typedef class Interface *InterfacePtr, *const InterfacePtrC;
typedef class Lex *LexPtr, *const LexPtrC;
typedef class LexNode *LexNodePtr, *const LexNodePtrC;
typedef class LittleCalculator *LittleCalculatorPtr, *const LittleCalculatorPtrC;
typedef class Method *MethodPtr, *const MethodPtrC;
typedef class Node *NodePtr, *const NodePtrC;
typedef class Object *ObjectPtr, *const ObjectPtrC;
typedef class Package *PackagePtr, *const PackagePtrC;
typedef class Scope *ScopePtr, *const ScopePtrC;
typedef class Scope *ScopePtr, *const ScopePtrC;
typedef class ScopeBuilder *ScopeBuilderPtr, *const ScopeBuilderPtrC;
typedef class Segment *SegmentPtr, *const SegmentPtrC;
typedef class StackVar *StackVarPtr, *const StackVarPtrC;
typedef class SyntaxBuilder *SyntaxBuilderPtr, *const SyntaxBuilderPtrC;
typedef class Variable *VariablePtr, *const VariablePtrC;

typedef Buffer ScriptNativeString;
typedef Sint64 ScriptNativeInt;
typedef Float64 ScriptNativeFloat;
typedef Bool ScriptNativeBool;

typedef Exception SegmentLinkerException;
typedef Exception SegmentException;
typedef Exception ScriptException;

class LAME_API SyntaxException : public Exception {
public:
	SyntaxException(
		Uint32 line,
		StringC message,
		...);
	SyntaxException(
		Bool warning,
		Uint32 line,
		StringC message, ...);
public:
	inline Uint32 Line() {
		return this->line_;
	}
	inline Bool IsWarning() {
		return this->warning_;
	}
public:
	Void Debug(PrintStreamPtr printer);
protected:
	Bool warning_;
	Uint32 line_;
};

typedef ScriptException ClassInvalidCastException;
typedef ScriptException MethodException;
typedef ScriptException InterfaceException;

#define PostSyntaxError(_line, _message, ...) \
	do { \
		SyntaxException e(_line, ""); \
		sprintf(e.GetErrorBuffer(), _message, __VA_ARGS__); \
		throw e; \
	} while (0);

#define PostSyntaxWarning(_line, _message, ...) \
    do { \
		static Bool __warningExceptionLock = 0; \
		if (!__warningExceptionLock || TRUE) { \
			SyntaxException(1, _line, _message, __VA_ARGS__)\
				.Debug(Console::GetInstance()); \
		} __warningExceptionLock = 1; \
    } while (0);

LAME_END2

#endif // ~__LAME_SCRIPT__DEFINE__