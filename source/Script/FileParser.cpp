#include <string>
#include <utility>

#ifdef LAME_CLANG
using std::swap;
#endif

#include <algorithm>
#include <fstream>

#include "FileParser.h"

LAME_BEGIN2(Script)

static StringC lockedTokens = "\n\r\t ";
static StringC allowedTokens = "@$_=/|&><+-*[].";
static StringC wordTokens = "_$@";
static StringC writeSpaces = "\a\b\t\n\r";

static LexPtrC _ParseLex(StringC* wordPtr, Buffer* name, Uint32 line, Bool isCommentLock) {

	StringC savedWord = *wordPtr;
	StringC word = *wordPtr;
	Buffer lexWord;
	LexPtr lex = LAME_NULL;
	Bool isWord;

	if (!isCommentLock && (*word == '\"' || *word == '\'')) {

		Bool isCharacterLike = (*word == '\'');

		while (*++word != *savedWord) {
			if (!*word || *word == '\n' || *word == '\r') {
				PostSyntaxError(line, "Quote or Apostrophe mismatched", 1);
			}
			lexWord.append(1, *word);
		}
		++word;

		if (isCharacterLike) {
			if (lexWord.length() == 2 && lexWord[0] == '\\') {
				Sint8 symbol = 0;
				switch (lexWord[1]) {
                    case 'n': symbol = '\n'; break;
                    case 'r': symbol = '\r'; break;
                    case 't': symbol = '\t'; break;
                    case 'v': symbol = '\v'; break;
                    case 'b': symbol = '\b'; break;
                    case 'a': symbol = '\a'; break;
				}
				lexWord.resize(1);
				lexWord[0] = symbol;
			}
			lex = (LexPtr)Lex::Find(kScriptLexCharacter);
		}
		else {
			lex = (LexPtr)Lex::Find(kScriptLexString);
		}

		goto __Return;
	}

	Bool isDigit = FileParser::IsDigit(*word) ||
		*word == '.';

	if (FileParser::IsFloatValue(word) && isDigit) {

		lex = Lex::Find(kScriptLexFloat);
		strtof(word, (String*)&word);

		if (*word == 'f') {
			++word;
		}
		goto __SaveWord;
	}
	else if (FileParser::IsHexValue(word) && isDigit) {

		lex = Lex::Find(kScriptLexInt);
		strtol(word + 2, (String*)&word, 16);

		goto __SaveWord;
	}
	else if (FileParser::IsIntValue(word) && isDigit) {

		lex = Lex::Find(kScriptLexInt);
		strtol(word, (String*)&word, 10);

		goto __SaveWord;
	}
	else if (strchr(lockedTokens, *word)) {
		++word;
		goto __Return;
	}
	else {
		isWord = FileParser::IsLetter(*word);

		auto l = Lex::Match(word);
        
        l.sort([](LexPtrC left, LexPtrC right) {
            return left->word.length() < right->word.length();
        });

		if (!l.empty() && !(FileParser::IsLetter(*(word + l.back()->word.length() + 1)))) {
			word += l.back()->word.length();
		}
		else {
			while (LAME_TRUE) {
				if (isWord) {
					if (!FileParser::IsLetter(*word) && !FileParser::IsDigit(*word) && !strchr(wordTokens, *word)) {
						break;
					}
				}
				else {
					if (!strchr(allowedTokens, *word)) {
						break;
					}
				}
				if (!*word) {
					break;
				}
				++word;
			}
			if (word == savedWord) {
				++word;
			}
		}
	}

	lexWord.append(savedWord, Sint32(word - savedWord));

	if (!lex) {

		lex = Lex::Find(lexWord);

		if (!lex) {
			lex = Lex::Find(kScriptLexUnknown);
		}
	}

__SaveWord:
	if (!lexWord.length()) {
		lexWord.append((String)savedWord, word - savedWord);
	}

__Return:
	*wordPtr = word;

	if (name) {
		*name = lexWord;
	}

	return LexPtrC(lex);
}

Void FileParser::Load(StringC fileName) {

	File handle;

	std::string line;
	std::ifstream stream;

	stream.open(fileName, std::ios::binary);

	if (!stream) {
		throw Exception("Unable to open file (%s)", fileName);
	}

	while (std::getline(stream, line)) {
		for (Sint8 c : line) {
			if (c != '\r') {
				this->fileText.push_back(c);
			}
		}
		this->fileText.append("\n");
	}

	this->Parse(this->fileText.data());

	FilePathInfo fpi(fileName);

	this->fileName = fpi.name;
	this->filePath = fpi.path;
	this->fileExtension = fpi.extension;
}

Void FileParser::Parse(StringC script) {

	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;
	Buffer lexWord;

	while (*script) {

		while (
			*(script) == ' ' ||
			*(script) == '\t' ||
			*(script) == '\r' ||
			*(script) == '\n'
        ) {
			if (*script == '\0') {
				goto __ExitLoop;
			}
			else if (*script == '\n') {
				++line;
			}
			script++;
		}

		LexNodePtr node = new LexNode(lexWord, line,
            _ParseLex(&script, &lexWord, line, isCommentLock));

		if (!node->lex) {
			delete node; break;
		}

		Uint32 extraWordLines = 0;
        
		for (auto i = lexWord.begin(); i != lexWord.end(); i++) {
			if (strchr(writeSpaces, *i)) {
				if (*i == '\n') {
					++extraWordLines;
				}
				i = lexWord.erase(i);
			}
		}

		if (extraWordLines > 0) {
			line += extraWordLines - 1;
		}

		if (node->lex->id == kScriptLexCommentL) {
			isCommentLock = LAME_TRUE;
			continue;
		}
		else if (node->lex->id == kScriptLexCommentR) {
			isCommentLock = LAME_FALSE;
			continue;
		}

		if (node->lex->id == kScriptLexLineComment) {
			++line;
			if (*(script - 1) == '\n') {
				continue;
			}
			while (*script && *script != '\n' && *script != '\r') {
				++script;
			}
			++script;
			continue;
		}

		if (isCommentLock) {
			continue;
		}

		if (node->lex->word.length()) {
			this->lexList.push_back(node);
		}
	}
__ExitLoop:
	;
}

Void FileParser::Clear(Void) {

	for (LexNodePtr n : this->lexList) {
		delete n;
	}
}

Bool FileParser::IsDigit(Sint8 symbol) {
	return symbol >= '0' && symbol <= '9';
}

Bool FileParser::IsLetter(Sint8 symbol) {
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z') ||
		strchr(wordTokens, symbol);
}

Bool FileParser::IsIntValue(StringC string) {

	String result;

	strtoll(string, &result, 10);

	return string != result &&
		isdigit(*string);
}

Bool FileParser::IsHexValue(StringC string) {

	String result;

	if (*(string + 0) == '0' && *(string + 1) == 'x') {
		strtoll(string + 2, &result, 16);
	}
	else {
		return LAME_FALSE;
	}

	return string + 2 != result;
}

Bool FileParser::IsStringValue(StringC string) {

	Uint32 length = (Uint32)strlen(string);

	if (length <= 1) {
		return LAME_FALSE;
	}

	return
		(string[0] == '\"' && string[length - 1] == '\"') ||
		(string[0] == '\'' && string[length - 1] == '\'');
}

Bool FileParser::IsIntValue(const Core::Buffer& string) {
	return IsIntValue(string.data());
}

Bool FileParser::IsFloatValue(StringC string) {

	String finish;
	StringC pointer;

	strtod(string, &finish);

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

Bool FileParser::IsFloatValue(const Core::Buffer& string) {

	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		return IsFloatValue(string.data());
	}
}

ScriptNativeInt FileParser::ParseIntValue(StringC string) {

	if (IsHexValue(string)) {
		return (ScriptNativeInt)strtoll(string, NULL, 16);
	}

	return (ScriptNativeInt)strtoll(string, NULL, 10);
}

ScriptNativeFloat FileParser::ParseFloatValue(StringC string) {
	return (ScriptNativeFloat)strtod(string, NULL);
}

ScriptNativeString FileParser::ParseStringValue(StringC string) {
	return string;
}

LAME_END2
