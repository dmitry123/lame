#ifndef __LAME_SCRIPT__EXCEPTION__
#define __LAME_SCRIPT__EXCEPTION__

#include "Define.h"

LAME_BEGIN2(Script)

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
	Void Debug();
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
	if (!__warningExceptionLock) { \
		SyntaxException(1, _line, _message, __VA_ARGS__).Debug(); \
	} __warningExceptionLock = 1; \
    } while (0);

LAME_END2

#endif // ~__LAME_SCRIPT__EXCEPTION__