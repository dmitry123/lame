#include "Error.h"

LAME_BEGIN2(Script)

namespace internal {
	static Uint32 lastError = Error::NoError;
	static Error::ErrorHandler errorHandler = NULL;
}

const StringC Error::description[] = {
	"Unknown error",
	"No error",
	"Class already extended",
	"Class already implemented with this one",
	"Class can implements only interfaces",
	"Unable to overload unknown operator",
	"Object must be variable",
	"Objects has different hash codes",
	"Class's operator not overloaded",
	"Invalid method arguments",
	"You can't override native method",
	"Not-static methods must have This",
	"Unable to clone object",
	"Unable to cast object",
	"Interface methods must't be implemented",
	"Class must implement interface's methods",
	"Class must implement interface's methods",
	"Abstract classes can't store implemented abstract methods",
	"Non-abstract objects must implement abstract methods",
	"Object must be array"
};

Error Error::GetLastError(Void) {
	return internal::lastError;
}

StringC Error::GetErrorDescription(Error error) {
	return error.GetDescription();
}

Void Error::SetLastError(ErrorPtr code) {

	internal::lastError = code->errorCode;

	if (internal::errorHandler != NULL && code->errorCode != NoError) {
		internal::errorHandler(*code);
	}
}

Void Error::SetErrorHandler(ErrorHandler handler) {
	internal::errorHandler = handler;
}

LAME_END2
