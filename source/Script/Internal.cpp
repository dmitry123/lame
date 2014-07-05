#include "Internal.h"

LAME_BEGIN2(Script)

Bool IsDigit(Sint8 symbol) {
	return symbol >= '0' && symbol <= '9';
}

Bool IsLetter(Sint8 symbol) {
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
}

Bool IsIntValue(StringC string) {

	String result;

	strtoll(string, &result, 10);

	return string != result && isdigit(*string);
}

Bool IsHexValue(StringC string) {

	String result;

	if (*(string + 0) == '0' && *(string + 1) == 'x') {
		strtoll(string + 2, &result, 16);
	}
	else {
		return LAME_FALSE;
	}

	return string + 2 != result;
}

Bool IsStringValue(StringC string) {

	Uint32 length = (Uint32)strlen(string);

	if (length <= 1) {
		return LAME_FALSE;
	}

	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

Bool IsIntValue(const Core::Buffer& string) {
	return IsIntValue(string.data());
}

Bool IsFloatValue(StringC string) {

	String finish;
	StringC pointer;

	if (sizeof(ScriptNativeFloat) == 4) {
		strtof(string, &finish);
	} else {
		strtod(string, &finish);
	}

	if (finish != string) {

		pointer = string;

		while (pointer != finish) {
			if (*(pointer++) == '.') {
				return LAME_TRUE;
			}
		}
	}
	return LAME_FALSE;
}

Bool IsFloatValue(const Core::Buffer& string) {

	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		return IsFloatValue(string.data());
	}
}

ScriptNativeInt ParseIntValue(StringC string) {
    
	if (IsHexValue(string)) {
        if (sizeof(ScriptNativeInt) == 8) {
            return (ScriptNativeInt) strtoll(string, NULL, 16);
        } else {
            return (ScriptNativeInt) strtol(string, NULL, 16);
        }
	}

    if (sizeof(ScriptNativeInt) == 8) {
        return (ScriptNativeInt) strtoll(string, NULL, 10);
    } else {
        return (ScriptNativeInt) strtol(string, NULL, 10);
    }
}

ScriptNativeFloat ParseFloatValue(StringC string) {

	if (sizeof(ScriptNativeFloat) == 4) {
		return (ScriptNativeFloat)strtof(string, NULL);
	} else {
		return (ScriptNativeFloat)strtod(string, NULL);
	}
}

ScriptNativeString ParseStringValue(StringC string) {
	return string;
}

LAME_END2
