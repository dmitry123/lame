#ifndef __LAME_LANG__ERROR_MESSAGE__
#define __LAME_LANG__ERROR_MESSAGE__

#include "Foundation.h"

LAME_LANG_BEGIN

typedef class ErrorMessage {
public:
	inline ErrorMessage():
		message(nullptr)
	{
	}
public:
	ErrorMessage(StringPtrC message, ...);
	ErrorMessage(const ErrorMessage& error);
	~ErrorMessage();
public:
	inline StringPtrC getMessage() const {
		return this->message;
	}
private:
	StringPtr message;
} *ErrorMessagePtr;

LAME_LANG_END

#endif // ~__LAME_LANG__ERROR_MESSAGE__