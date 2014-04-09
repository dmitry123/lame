#ifndef __LAME_SCRIPT__EXCEPTION__
#define __LAME_SCRIPT__EXCEPTION__

#include "Define.h"

LAME_BEGIN

class SyntaxException : public Exception {
public:
	SyntaxException(Uint32 line, StringC message, ...) {
		this->warning_ = 0;
		this->line_ = line;
		VaList list;
		va_start(list, message);
		vsprintf(this->buffer_, message, list);
		va_end(list);
	}
	SyntaxException(Bool, Uint32 line, StringC message, ...) {
		this->warning_ = 1;
		this->line_ = line;
		VaList list;
		va_start(list, message);
		vsprintf(this->buffer_, message, list);
		va_end(list);
	}
public:
	inline Uint32 Line() {
		return this->line_;
	}
	inline Void Debug() {
		if (this->warning_) {
			printf("Syntax Warning (%d) : %s\n", this->line_, this->buffer_);
		}
		else {
			printf("Syntax Error (%d) : %s", this->line_, this->buffer_);
		}
	}
	inline Bool IsWarning() {
		return this->warning_;
	}
protected:
	Bool warning_;
	Uint32 line_;
};

#define PostSyntaxError(_line, _message, ...) \
	throw SyntaxException(_line, _message, __VA_ARGS__)

#define PostSyntaxWarning(_line, _message, ...) \
	static Bool __warningExceptionLock = 0; \
	if (!__warningExceptionLock) { \
		SyntaxException(1, _line, _message, __VA_ARGS__).Debug(); \
		} __warningExceptionLock = 1;

LAME_END

#endif // ~__LAME_SCRIPT__EXCEPTION__