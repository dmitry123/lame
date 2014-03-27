#include "Parser.h"
#include "Type.h"

LAME_BEGIN

inline Bool IsDigit(Sint8 symbol) {
	return symbol >= '0' && symbol <= '9';
}

inline Bool IsLetter(Sint8 symbol) {
	return
		symbol >= 'a' && symbol <= 'z' ||
		symbol >= 'A' && symbol <= 'Z';
}

inline Bool IsIntValue(StringC string) {

	Uint32 length = strlen(string);

	for (Uint32 i = 0; i < length; i++) {
		if (!IsDigit(string[i])) {
			return LAME_FALSE;
		}
	}
	return LAME_TRUE;
}

inline Bool IsStringValue(StringC string) {

	Uint32 length = strlen(string);

	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

inline Bool IsIntValue(BufferRefC string) {

	for (char s : string) {
		if (!IsDigit(s)) {
			return LAME_FALSE;
		}
	}
	return LAME_TRUE;
}

inline Bool IsFloatValue(StringC string) {

	Sint8P finish;

	if (!string) {
		return LAME_FALSE;
	}

	Float32 result = strtof(string, &finish);

	if (*finish == 'f' && *(finish + 1) == '\0' || !*finish) {
		return LAME_TRUE;
	}

	return LAME_FALSE;
}

inline Bool IsFloatValue(BufferRefC string) {

	if (!string.length()) {
		return LAME_FALSE;
	} else {
		return IsFloatValue(string.data());
	}
}

inline Bool IsValue(BufferRefC string) {
	return IsIntValue(string) || IsFloatValue(string);
}

inline Sint32 ParseIntValue(BufferRefC string) {
	return atoi(string.data());
}

inline Float32 ParseFloatValue(BufferRefC string) {
	return strtof(string.data(), NULL);
}

inline Buffer ParseStringValue(StringC string) {
	Buffer result = string + 1;
	result[result.length() - 1] = '\0';
	return result;
}

static StringC allowedTokens = "'!~*/%+-&^|=\"\n\r\t ;,.:-{}()[]";
static StringC compareTokens = "<>=!";
static StringC separateTokens = ";{}";

KeyWord& KeyWord::Parse(Uint32 line, StringC word) {

	this->line = line;
	this->word = word;

	if (strchr(separateTokens, *word)) {
		this->key = kKeyWordSeparator;
		this->priority = kPriorityVariable;
		goto __Return;
	} else if (IsIntValue(word)) {
		this->key = kKeyWordInt;
		this->priority = kPriorityVariable;
		this->intValue = ParseIntValue(word);
		goto __Return;
	} else if (IsFloatValue(word)) {
		this->key = kKeyWordFloat;
		this->priority = kPriorityVariable;
		this->floatValue = ParseFloatValue(word);
		goto __Return;
	} else if (IsStringValue(word)) {
		this->key = kKeyWordString;
		this->priority = kPriorityVariable;
		this->stringValue = ParseStringValue(word);
		goto __Return;
	} else if ((this->priority = ParseLanguagePriority(word)) != kPriorityDefault) {
		this->key = kKeyWordPriority;
		goto __Return;
	} else if ((this->languageValue.Parse(word) != kScriptLanguageDefault)) {
		this->key = kKeyWordLanguage;
		this->priority = kPriorityLanguage;
		goto __Return;
	} else {
		this->key = kKeyWordDefault;
		this->priority = kPriorityDefault;
		goto __Return;
	}

__Return:
	return *this;
}

Void ScriptParser::Parse(StringC script) {

	Uint32 line = 0;
	Sint8 token = 0;
	Buffer word;
	StringC last = script;

	Bool isToken = 0;
	Bool isParseToken = 0;
	Bool isQuoteLock = 0;
	Bool isApostropheLock = 0;
	Bool isCommentLock = 0;

	while (*script) {

		token = *script;

		isToken = 0;
		isParseToken = 0;

		if (*(script + 1)) {
			if (!isCommentLock) {
				if (*script == '/' && *(script + 1) == '*') {
					isCommentLock = 1; goto __Continue;
				}
			} else {
				if (*script == '*' && *(script + 1) == '/') {
					isCommentLock = 0; goto __Continue;
				}
			}
		}

		if (token == '\"') {
			if (!isApostropheLock) {
				isQuoteLock = !isQuoteLock; goto __Skip;
			}
		} else if (token == '\'') {
			if (!isQuoteLock) {
				isApostropheLock = !isApostropheLock; goto __Skip;
			}
		} else {
			if (isQuoteLock || isApostropheLock || isCommentLock) {
				goto __Skip;
			}
		}

		if (token == '\n' || token == '\r') {
			if (token == '\n') {
				line++;
			}
			token = ' ';
		} else if (token == '\t') {
			token = ' ';
		}

		if (!strchr(allowedTokens, token)) {
			if (token != ' ') {
				goto __Skip;
			}
		} else {
			if (!strchr(compareTokens, token)) {
				if (*(script + 1) != '=' && token != '<' && token != '>') {
					goto __SaveNonSpaceToken;
				}
			} else {
			__SaveNonSpaceToken:
				if (token != ' ') {
					isToken = 1;
				}
			}
		}

		if (isToken) {
			if (script - last > 0) {
				isParseToken = 1;
				isToken = 0;
			}
		}

		if (isToken || script - last > 0) {
		__ParseToken:
			word.clear();
			if (isToken) {
				word.append(last, script - last, 1);
			} else {
				word.append(last, script - last);
			}
			this->keyQueue.push_back(
				KeyWord().Parse(line, word)
			);
		}

		if (isParseToken) {
			isParseToken = 0;
			isToken = 1;
			goto __ParseToken;
		}

	__Continue:
		last = script + 1;
	__Skip:
		++script;
	}

	for (auto& k : this->keyQueue) {
		printf("(%d, %d) : %s\n", k.key, k.priority, k.word.data());
	}
}

LAME_END