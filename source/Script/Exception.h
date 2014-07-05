#ifndef __LAME_SCRIPT__EXCEPTION__
#define __LAME_SCRIPT__EXCEPTION__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API SyntaxException : public ::LAME_NAMESPACE::Core::Exception {
public:
	SyntaxException(
		Uint32 line,
		Uint32 where,
		StringC message,
		...);
	SyntaxException(
		Bool warning,
		Uint32 line,
		Uint32 where,
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
    Uint32 where_;
};

typedef ::Lame::Core::Exception ClassInvalidCastException;
typedef ::Lame::Core::Exception ArrayOutOfBoundsException ;

#define PostSyntaxError(_line, _message, ...) \
	throw SyntaxException(_line, __LINE__, _message, __VA_ARGS__)

#define PostSyntaxWarning(_line, _message, ...) \
	static Bool __warningExceptionLock = 0; \
	if (!__warningExceptionLock) { \
		SyntaxException(1, _line, __LINE__, _message, __VA_ARGS__).Debug(); \
	} __warningExceptionLock = 1;

LAME_END2

#endif // ~__LAME_SCRIPT__EXCEPTION__