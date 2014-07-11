#include "FileParser.h"
#include "Exception.h"
#include "Internal.h"

LAME_BEGIN2(Script)

static LexPtrC _ParseLex(StringC* wordPtr, Buffer* name) {

	static StringC lockedTokens = "\n\r\t ";
	static StringC allowedTokens = "@$_=/|&><+-*[]";
	static StringC wordTokens = "_$";

	StringC savedWord = *wordPtr;
	StringC word = *wordPtr;
	Buffer lexWord;
	LexPtr lex = LAME_NULL;
	Bool isWord;

	if (*word == '\"' || *word == '\'') {

		while (*++word != *savedWord) {
			if (!*word) {
				PostSyntaxError(0, "Quote or Apostrophe mismatched", 1);
			}
			lexWord.append(1, *word);
		}
		++word;

		lex = (LexPtr) Lex::Find(kScriptLexString);

		goto __Return;
	}

	if (IsFloatValue(word)) {

		lex = Lex::Find(kScriptLexFloat);
		strtof(word, (String*)&word);

		if (*word == 'f') {
			++word;
		}
		goto __SaveWord;
	}
	else if (IsHexValue(word)) {

		lex = Lex::Find(kScriptLexInt);
		strtol(word + 2, (String*)&word, 16);

		goto __SaveWord;
	}
	else if (IsIntValue(word)) {

		lex = Lex::Find(kScriptLexInt);
		strtol(word, (String*)&word, 10);

		goto __SaveWord;
	}
	else if (strchr(lockedTokens, *word)) {
		++word;
		goto __Return;
	}
	else {
		isWord = IsLetter(*word);

		while (LAME_TRUE) {
			if (isWord) {
				if (!IsLetter(*word) && !IsDigit(*word) && !strchr(wordTokens, *word)) {
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

	lexWord.append(savedWord, Sint32(word - savedWord));

	if (!lex) {

		lex = Lex::Find(lexWord);

		if (!lex) {
			lex = Lex::Find(kScriptLexDefault);
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
	Buffer script;

	handle.Open(fileName, "rt");

	script.resize(handle.GetSize());

	handle.Read((String)script.data(), handle.GetSize());
	handle.Close();
	
	this->Parse(script.data());

	script.clear();
}

Void FileParser::Parse(StringC script) {

	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;
	Buffer lexWord;

	while (*script) {

		LexNodePtr node = new LexNode(lexWord, line, _ParseLex(&script, &lexWord));

		if (!node->lex) {
			++line;
			delete(node);
			continue;
		}

		while (
			*(script) == ' ' ||
			*(script) == '\t' ||
			*(script) == '\r' ||
			*(script) == '\n'
		) {
			if (*script == '\0') {
				goto __ExitLoop;
			}
			else if (*script == '\n' || *script == '\r') {
				++line;
			}
			script++;
		}

		if (node->lex->id == kScriptLexCommentL) {
			isCommentLock = LAME_TRUE; continue;
		}
		else if (node->lex->id == kScriptLexCommentR) {
			isCommentLock = LAME_FALSE; continue;
		}

		if (node->lex->id == kScriptLexLineComment) {

			if (*(script - 1) == '\n' || *(script - 1) == '\r') {
				continue;
			}
			while (*script && *script != '\n') {
				++script;
			}

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

LAME_END2
