#ifndef __LAME_LANG__SYNTAX_ERROR__
#define __LAME_LANG__SYNTAX_ERROR__

#include "Foundation.h"
#include "ErrorMessage.h"

#include <exception>

LAME_LANG_BEGIN

typedef class SyntaxError : public std::exception {
public:
	typedef enum { 
		UNKNOWN,
		TOKEN_MISMATCH,
		INVALID_TOKEN,
		ILLEGAL_PLACE
	} Code;
public:
	SyntaxError(UInt32 code, UInt32 line = 0) :
		code(code),
		error(ErrorMessage()),
		line(line)
	{
	}
	SyntaxError(UInt32 code, ErrorMessage error, UInt32 line = 0):
		code(code),
		error(error),
		line(line)
	{
	}
public:
    char const* what() const noexcept {
		return this->getMessage();
	}
public:
	inline StringPtrC getMessage() const {
		if (this->error.getMessage() != nullptr) {
			return this->error.getMessage();
		}
		else {
			return "Syntax error";
		}
	}
	inline UInt32 getCode() const {
		return this->code;
	}
	inline ErrorMessagePtr getError() {
		return &this->error;
	}
	inline UInt32 getLine() const {
		return this->line;
	}
private:
	ErrorMessage error;
private:
	UInt32 code;
	UInt32 line;
} *SyntaxErrorPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__SYNTAX_ERROR__