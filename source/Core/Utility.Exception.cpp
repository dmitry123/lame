#include "Utility.h"

LAME_BEGIN2(Core)

Exception::Exception(
	StringC file,
	Uint32 line,
	StringC function,
	StringC message, ...) {

	VaList list;
	ErrorBuffer subbuf;

	va_start(list, message);
	vsprintf(subbuf, message, list);
	va_end(list);

	StringC filename = file;

	while (strchr(filename + 1, LAME_SLASH) && (filename = strchr(filename + 1, LAME_SLASH))) {
		// seeking
	}

	++filename;

	sprintf(this->buffer_, "%s : %s (%d) : [%s]", filename, function, line, subbuf);
}

LAME_END2
