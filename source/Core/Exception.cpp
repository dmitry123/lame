#include "Exception.h"

#include <stdio.h>

LAME_BEGIN2(Core)

Exception::Exception(StringC message, ...) {

	VaList vaList;
	va_start(vaList, message);
	vsprintf(this->GetErrorBuffer(), message, vaList);
	va_end(vaList);
}

LAME_END2