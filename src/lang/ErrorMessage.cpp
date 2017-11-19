#include "ErrorMessage.h"

#include <cstdarg>

LAME_LANG_BEGIN

ErrorMessage::ErrorMessage(StringPtrC message, ...) {
	this->message = new char[LAME_ERROR_BUFFER + 1];
	va_list list;
	va_start(list, message);
	vsnprintf(this->message, LAME_ERROR_BUFFER, message, list);
	va_end(list);
}

ErrorMessage::ErrorMessage(const ErrorMessage& error):
	message(strdup(error.message))
{
}

ErrorMessage::~ErrorMessage() {
	delete this->message;
}

LAME_LANG_END