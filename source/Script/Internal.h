#ifndef __LAME_SCRIPT__INTERNAL__
#define __LAME_SCRIPT__INTERNAL__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN

static Bool IsDigit(Sint8 symbol) {
	// cheking for digit
	return symbol >= '0' && symbol <= '9';
}

static Bool IsLetter(Sint8 symbol) {
	// checking for letter
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
}

static Bool IsIntValue(StringC string) {
	String result;
	// parsing word
	strtoll(string, &result, 10);
	// checking for int
	return string != result && isdigit(*string);
}

static Bool IsHexValue(StringC string) {
	String result;
	// checking for hex and parsing
	if (*(string + 0) == '0' && *(string + 1) == 'x') {
		strtoll(string + 2, &result, 16);
	}
	else {
		return LAME_FALSE;
	}
	// checking for ehx
	return string + 2 != result;
}

static Bool IsStringValue(StringC string) {
	Uint32 length = (Uint32)strlen(string);
	// checking string's length
	if (length <= 1) {
		return LAME_FALSE;
	}
	// checking for string
	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

static Bool IsIntValue(const Buffer& string) {
	return IsIntValue(string.data());
}

static Bool IsFloatValue(StringC string) {
	String finish;
	StringC pointer;
	// parsing word
	strtof(string, &finish);
	// checking result
	if (finish != string) {
		// getting pointer
		pointer = string;
		// looking for dots
		while (pointer != finish) {
			if (*(pointer++) == '.') {
				return LAME_TRUE;
			}
		}
	}
	return LAME_FALSE;
}

static Bool IsFloatValue(const Buffer& string) {
	// checking for length
	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		// parsing float
		return IsFloatValue(string.data());
	}
}

static ScriptNativeInt ParseIntValue(StringC string) {
	// if value hex then parse as hex number
	if (IsHexValue(string)) {
		return strtoll(string, NULL, 16);
	}
	// else parse as normal number
	return strtoll(string, NULL, 10);
}

static ScriptNativeFloat ParseFloatValue(StringC string) {
	// parsing float value
	return strtof(string, NULL);
}

static ScriptNativeString ParseStringValue(StringC string) {
	// parse string value
	return string;
}

LAME_END

#endif // ~__LAME_SCRIPT__INTERNAL__