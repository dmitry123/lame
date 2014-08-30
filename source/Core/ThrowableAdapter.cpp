#include "ThrowableAdapter.h"

LAME_BEGIN2(Core)

ErrorBuffer errorBuffer = {
	0
};

String ThrowableAdapter::GetErrorBuffer() {
	return errorBuffer;
}

LAME_END2