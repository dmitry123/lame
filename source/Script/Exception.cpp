#include "Exception.h"

LAME_BEGIN2(Script)

SyntaxException::SyntaxException(Uint32 line, Uint32 where, StringC message, ...) {

	this->warning_ = 0;
	this->line_ = line;
	this->where_ = where;
	VaList list;
	va_start(list, message);
	vsprintf(this->buffer_, message, list);
	va_end(list);
}

SyntaxException::SyntaxException(Bool, Uint32 line, Uint32 where, StringC message, ...) {

	this->warning_ = 1;
	this->line_ = line;
	this->where_ = where;
	VaList list;
	va_start(list, message);
	vsprintf(this->buffer_, message, list);
	va_end(list);
}

Void SyntaxException::Debug() {

	if (this->warning_) {
		printf("Syntax Warning (%d) : %s\n", this->line_, this->buffer_);
	}
	else {
		printf("Syntax Error (%d) : %s", this->line_, this->buffer_);
	}
}

LAME_END2
