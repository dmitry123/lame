#include "Console.h"

#include <stdio.h>

LAME_BEGIN2(Core)

Void Console::Print() {
	printf("\n");
}

Void Console::Print(StringC format, ...) {

	VaList vaList;

	va_start(vaList, format);
	vprintf(format, vaList);
	va_end(vaList);
}

Void Console::Skip(Uint32 length) {
	for (Uint32 i = 0; i < length; i++) {
		printf(" ");
	}
}

Uint32 Console::Available() {
	return -1;
}

Void Console::Close() {
}

Void Console::Reset() {
}

ConsolePtr Console::consolePrinter
	= new Console();

LAME_END2