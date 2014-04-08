#ifndef __LAME_SCRIPT__INTERNAL__
#define __LAME_SCRIPT__INTERNAL__

#include "Define.h"

LAME_BEGIN

static inline Bool IsDigit(Sint8 symbol) {
	return symbol >= '0' && symbol <= '9';
}

static inline Bool IsLetter(Sint8 symbol) {
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
}

static inline Bool IsIntValue(StringC string) {
	String result;
	strtoll(string, &result, 10);
	if (string != result && isdigit(*string)) {
		return LAME_TRUE;
	}
	return LAME_FALSE;
}

static inline Bool IsHexValue(StringC string) {
	String result;
	if (*(string + 0) == '0' &&
		*(string + 1) == 'x') {
		strtoll(string + 2, &result, 16);
	}
	else {
		return LAME_FALSE;
	}
	if (string + 2 != result) {
		return LAME_TRUE;
	}
	return LAME_FALSE;
}

static inline Bool IsStringValue(StringC string) {
	Uint32 length = (Uint32)strlen(string);
	if (length <= 1) {
		return LAME_FALSE;
	}
	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

static inline Bool IsIntValue(const Buffer& string) {
	return IsIntValue(string.data());
}

static inline Bool IsFloatValue(StringC string) {
	String finish;
	StringC pointer;
	if (!string) {
		return LAME_FALSE;
	}
	strtof(string, &finish);
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

static inline Bool IsFloatValue(const Buffer& string) {
	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		return IsFloatValue(string.data());
	}
}

static inline ScriptNativeInt ParseIntValue(const Buffer& string) {
	if (IsHexValue(string.data())) {
		return strtoll(string.data(), NULL, 16);
	}
	return strtoll(string.data(), NULL, 10);
}

static inline ScriptNativeInt ParseIntValue(StringC string) {
	if (IsHexValue(string)) {
		return strtoll(string, NULL, 16);
	}
	return strtoll(string, NULL, 10);
}

static inline ScriptNativeFloat ParseFloatValue(StringC string) {
	return strtof(string, NULL);
}

static inline ScriptNativeFloat ParseFloatValue(const Buffer& string) {
	return strtof(string.data(), NULL);
}

static inline ScriptNativeString ParseStringValue(StringC string) {
	Buffer result = string + 1;
	result[result.length() - 1] = '\0';
	return result;
}

LAME_END

#endif // ~__LAME_SCRIPT__INTERNAL__