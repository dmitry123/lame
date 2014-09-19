#include "Define.h"

LAME_BEGIN2(Script)

SyntaxException::SyntaxException(Uint32 line, StringC message, ...) : Exception(message) {

	this->warning_ = 0;
	this->line_ = line;
	VaList list;
	va_start(list, message);
	vsprintf(this->GetErrorBuffer(), message, list);
	va_end(list);
}

SyntaxException::SyntaxException(Bool, Uint32 line, StringC message, ...) : Exception(message) {

	this->warning_ = 1;
	this->line_ = line;
	VaList list;
	va_start(list, message);
	vsprintf(this->GetErrorBuffer(), message, list);
	va_end(list);
}

Void SyntaxException::Debug(PrintStreamPtr printer) {

	if (this->warning_) {
		printf("Syntax Warning (%d) : %s\n", this->line_, this->GetErrorBuffer());
	}
	else {
		printf("Syntax Error (%d) : %s", this->line_, this->GetErrorBuffer());
	}
}

LAME_END2
