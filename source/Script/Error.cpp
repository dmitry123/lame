#include "Error.h"

LAME_BEGIN2(Script)

namespace internal {
	static Uint32 lastError = ScriptError::NoError;
	static ScriptError::ErrorHandler errorHandler = NULL;
}

const StringC ScriptError::description[] = {
	"Unknown error",
	"No error",
	"Class already extended",
	"Class already implemented with this one"
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
	"Abstract can't store implemented abstract methods",
	"Non-abstract objects must implement abstract methods"
};

ScriptError ScriptError::GetLastError(Void) {
	return internal::lastError;
}

StringC ScriptError::GetErrorDescription(ScriptError error) {
	return error.GetDescription();
}

Void ScriptError::SetLastError(ScriptErrorPtr code) {

	internal::lastError = code->errorCode;

	if (internal::errorHandler != NULL && code->errorCode != NoError) {
		internal::errorHandler(*code);
	}
}

Void ScriptError::SetErrorHandler(ErrorHandler handler) {
	internal::errorHandler = handler;
}

LAME_END2