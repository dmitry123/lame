#include "ScriptParser.h"

LAME_LANG_BEGIN

StringPtrC ScriptParser::whitespaces = "\r\n\t ";
StringPtrC ScriptParser::letters = "_$";
StringPtrC ScriptParser::tokens = "+-*/=><%~!&|^?:{}()[];@,";

Logic ScriptParser::parse(StringPtrC code) {

	Logic comment = false,
		line = false;

	StringPtrC ptr = code,
		word = nullptr,
		last = nullptr;

	UInt32 length;

	this->line = 1;

	while (*ptr) {

		TokenPtr lex = nullptr;
		Char token = *ptr++;

		if (token == '\n') {
			++this->line;
		}

		if (!line && (token == '/' && *ptr == '/') ||
			line && (token == '\n' || token == '\r')
		) {
			line = !line;
			continue;
		}
		else if (token == '/' && *ptr == '*' ||
			token == '*' && *ptr == '/'
		) {
			comment = token == '/';
			++ptr;
			continue;
		}
		else if ((token == '"' || token == '\'') && (length = this->parseString(ptr, lex)) > 0) {
			word = ptr - length--;
		}
		else if (this->isWhitespace(token) != false || comment || line) {
			continue;
		}
		else {
			if (
				this->isLetter(token) && (length = this->parseWord(ptr, lex)) > 0 ||
				this->isToken(token) && (length = this->parseToken(ptr, lex)) > 0 ||
				this->isDigit(token) && (length = this->parseNumber(ptr, lex)) > 0
			) {
				word = ptr - length;
			}
			else {
				continue;
			}
		}

		word = (StringPtrC)strncpy((StringPtr)calloc(1, length + 1), word, length);

		if (!lex && !(lex = this->getFinder()->findByWord(word))) {
			lex = this->getFinder()->findByWord("<identifier>");
		}

		this->collection.push_back(new Node(lex, word, this->line));
	}

	return true;
}

UInt32 ScriptParser::parseWord(StringPtrC& ptr, TokenPtr& token) {

	StringPtrC src = ptr;

	while (*ptr && (this->isLetter(*ptr) || this->isDigit(*ptr))) {
		ptr++;
	}

	return ptr - src + 1;
}

UInt32 ScriptParser::parseNumber(StringPtrC& ptr, TokenPtr& token) {

	StringPtr src;
	UInt32 length;

	if (*ptr != 'x') {
		strtol(ptr - 1, &src, 10);
		if (*src == '.') {
			strtof(ptr - 1, &src);
			length = src - ptr + 1;
			if (*src == 'f') {
				++length;
				++src;
			}
			token = this->getFinder()->findByWord("<float>");
		}
		else {
			length = src - ptr + 1;
			token = this->getFinder()->findByWord("<integer>");
		}
	}
	else {
		strtol(ptr - 1, &src, 16);
		length = src - ptr + 1;
		token = this->getFinder()->findByWord("<int>");
	}
	ptr = src;

	return length;
}

UInt32 ScriptParser::parseToken(StringPtrC& ptr, TokenPtr& token) {

	static StringPtrC alone = "!?:{}()[];@,";

	StringPtrC src = ptr;

	while (*ptr && this->isToken(*ptr)) {
		if (strchr(alone, *ptr)) {
			break;
		}
		ptr++;
	}

	return ptr - src + 1;
}

UInt32 ScriptParser::parseString(StringPtrC& ptr, TokenPtr& token) {

	StringPtrC src = ptr;
	Char last = *(ptr - 1);

	while (*ptr && *ptr != last) {
		if (*ptr == '\n') {
			++this->line;
		}
		ptr++;
	}

	if (*ptr == '\0') {
		throw SyntaxError(SyntaxError::TOKEN_MISMATCH, "Quote or apostrophe mismatched");
	}

	if (last == '"') {
		token = this->getFinder()->findByWord("<string>");
	}
	else {
		token = this->getFinder()->findByWord("<char>");
	}

	return ptr++ - src + 1;
}

LAME_LANG_END