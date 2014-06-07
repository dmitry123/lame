#include "Parser.h"
#include "Internal.h"

LAME_BEGIN

#define P(_p) Uint32(20-_p)

static const ScriptLex scriptObjects[] = {
	// something unknown likes variable/function/type
	{ "${default}", P(15), kScriptLexDefault, 1,
		kScriptLexFlagLeft | kScriptLexFlagUnknown },
	// decimal or hexadecimal number
	{ "${int}", P(15), kScriptLexInt, 1,
		kScriptLexFlagRight | kScriptLexFlagConst },
	// float number with/without last 'f' symbol
	{ "${float}", P(15), kScriptLexFloat, 1,
		kScriptLexFlagRight | kScriptLexFlagConst },
	// const string separated by apostophe or quote
	{ "${string}", P(15), kScriptLexString, 1,
		kScriptLexFlagRight | kScriptLexFlagConst },
	// implements language word
	{ "implements", P(1), kScriptLexImplements, 0,
		kScriptLexFlagLanguage },
	// protected modificator
	{ "protected", P(0), kScriptLexProtected, 1,
		kScriptLexFlagLanguage | kScriptLexFlagModificator },
	// continue language word
	{ "continue", P(1), kScriptLexContinue, 0,
		kScriptLexFlagLanguage },
	// private modificator
	{ "private", P(0), kScriptLexPrivate, 1,
		kScriptLexFlagLanguage | kScriptLexFlagModificator },
	// sizeof language word
	{ "sizeof", P(15), kScriptLexSizeof, 1,
		kScriptLexFlagRight },
	// public modificator
	{ "public", P(0), kScriptLexPublic, 1,
	kScriptLexFlagLanguage | kScriptLexFlagModificator },
	// static modificator
	{ "static", P(0), kScriptLexStatic, 1,
	kScriptLexFlagLanguage | kScriptLexFlagModificator },
	// native modificator
	{ "native", P(0), kScriptLexNative, 1,
	kScriptLexFlagLanguage | kScriptLexFlagModificator },
    // return word
    { "return", P(15), kScriptLexReturn, 1,
		kScriptLexFlagLanguage | kScriptLexFlagRight },
	{ "final", P(0), kScriptLexFinal, 1,
		kScriptLexFlagLanguage | kScriptLexFlagModificator },
	// class language word
	{ "class", P(15), kScriptLexClass, 0,
		kScriptLexFlagLanguage },
	// break language word
	{ "break", P(1), kScriptLexBreak, 0,
		kScriptLexFlagLanguage },
	// while language word
	{ "while", P(14), kScriptLexWhile, 1,
		kScriptLexFlagCondition | kScriptLexFlagLanguage },
	// else language word
	{ "else", P(14), kScriptLexElse, 0,
		kScriptLexFlagCondition | kScriptLexFlagLanguage },
	// new language word
	{ "new", P(20), kScriptLexNew, 1,
		kScriptLexFlagRight | kScriptLexFlagLanguage | kScriptLexFlagLanguage },
	// for language word
	{ "for", P(14), kScriptLexFor, 1,
		kScriptLexFlagCondition | kScriptLexFlagLanguage },
	// do language word
	{ "do", P(14), kScriptLexDo, 0,
		kScriptLexFlagCondition | kScriptLexFlagLanguage },
	// if language word
	{ "if", P(14), kScriptLexIf, 1,
		kScriptLexFlagCondition | kScriptLexFlagLanguage },
	// left-associated left shift
	{ "<<=", P(2), kScriptLexBitShiftSetL, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated right shift
	{ ">>=", P(2), kScriptLexBitShiftSetR, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// mediated selection for objects
	{ "->", P(16), kScriptLexMediated, 2,
		kScriptLexFlagRight },
	// increment
	{ "++", P(15), kScriptLexIncrement, 1,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// decrement
	{ "--", P(15), kScriptLexDecrement, 1,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left shift
	{ "<<", P(11), kScriptLexBitShiftL, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// right shift
	{ ">>", P(11), kScriptLexBitShiftR, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// bellow or equal
	{ "<=", P(10), kScriptLexBellowEqual, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// above or equal
	{ ">=", P(10), kScriptLexAboveEqual, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// equal
	{ "==", P(9), kScriptLexEqual, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// not equal
	{ "!=", P(9), kScriptLexNotEqual, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// boolean and
	{ "&&", P(5), kScriptLexAnd, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// boolean or
	{ "||", P(4), kScriptLexOr, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// left-associated set
	{ "=", P(2), kScriptLexSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated add
	{ "+=", P(2), kScriptLexAddSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated sub
	{ "-=", P(2), kScriptLexSubSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated mul
	{ "*=", P(2), kScriptLexMulSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated div
	{ "/=", P(2), kScriptLexDivSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated mod
	{ "%=", P(2), kScriptLexModSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated and
	{ "&=", P(2), kScriptLexBitAndSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated xor
	{ "^=", P(2), kScriptLexBitXorSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// left-associated or
	{ "|=", P(2), kScriptLexBitOrSet, 2,
		kScriptLexFlagLeft | kScriptLexFlagMath },
	// begin comment
	{ "/*", P(1), kScriptLexCommentL, 0,
		kScriptLexFlagDefault },
	// end comment
	{ "*/", P(1), kScriptLexCommentR, 0,
		kScriptLexFlagDefault },
	// begin line-comment (ends with /n/r/0)
	{ "//", P(1), kScriptLexLineComment, 0,
		kScriptLexFlagDefault },
	// coma symbol
	{ ",", P(1), kScriptLexComa, 2,
		kScriptLexFlagRight },
	// left bracket
	{ "[", P(16), kScriptLexBracketL, 1,
		kScriptLexFlagDefault },
	// right bracket
	{ "]", P(16), kScriptLexBracketR, 1,
		kScriptLexFlagDefault },
	// left parenthese
	{ "(", P(0), kScriptLexParentheseL, 1,
		kScriptLexFlagDefault },
	// right parenthese
	{ ")", P(0), kScriptLexParentheseR, 1,
		kScriptLexFlagDefault },
	// directed selection
	{ ".", P(16), kScriptLexDirected, 2,
		kScriptLexFlagRight },
	// bit not
	{ "~", P(15), kScriptLexBitNot, 1,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// logic not
	{ "!", P(15), kScriptLexNot, 1,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// unary minus
	{ ":", P(15), kScriptLexUnaryMinus, 1,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// unary plus
	{ ":", P(15), kScriptLexUnaryPlus, 1,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// mul
	{ "*", P(13), kScriptLexMul, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// div
	{ "/", P(13), kScriptLexDiv, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// mod
	{ "%", P(13), kScriptLexMod, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// add
	{ "+", P(12), kScriptLexAdd, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// sub
	{ "-", P(12), kScriptLexSub, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// bellow
	{ "<", P(10), kScriptLexBellow, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// above
	{ ">", P(10), kScriptLexAbove, 2,
		kScriptLexFlagRight | kScriptLexFlagBool },
	// and
	{ "&", P(8), kScriptLexBitAnd, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// xor
	{ "^", P(7), kScriptLexBitXor, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// or
	{ "|", P(6), kScriptLexBitOr, 2,
		kScriptLexFlagRight | kScriptLexFlagMath },
	// quote
	{ "\"", P(1), kScriptLexQuote, 0,
		kScriptLexFlagDefault },
	// apostrophe
	{ "\'", P(1), kScriptLexApostrophe, 0,
		kScriptLexFlagDefault },
	// semicolon
	{ ";", P(1), kScriptLexSemicolon, 0,
		kScriptLexFlagDefault },
	// left brace
	{ "{", P(1), kScriptLexBraceL, 0,
		kScriptLexFlagDefault },
	// right brace
	{ "}", P(1), kScriptLexBraceR, 0,
		kScriptLexFlagDefault }
};

Void ScriptParser::Load(StringC file) {

	File handle;
	Buffer script;

	handle.Open(file, "rt");
	script.resize(handle.GetSize());
	handle.Read((String)script.data(), handle.GetSize());
	handle.Close();
	this->Parse(script.data());
	script.clear();

	/*for (ScriptLexNodePtr lex : this->lexList_) {
		printf("[%s]\n", lex->word.data());
	}*/
}

Void ScriptParser::Parse(StringC script) {

	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;
	Buffer lexWord;

	while (*script) {

		ScriptLexNodePtr node = new ScriptLexNode(lexWord, line, this->_ParseLex(&script, &lexWord));

		if (!node->lex) {
			delete(node); continue;
		}

		while (
			*(script) == ' ' ||
			*(script) == '\t' ||
			*(script) == '\r' ||
			*(script) == '\n'
		) {
			if (!*script) {
				goto __ExitLoop;
			}
			if (*script == '\n' || *script == '\r') {
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
			this->lexList_.push_back(node);
		}
	}
__ExitLoop:
	;
}

ScriptLexPtrC ScriptParser::Find(ScriptLexPtr lex, ScriptLexID id) {

	Uint32 lines = lex ? lex->line : 0;

	lex = LAME_NULL;

	for (Uint32 i = 0; i < kScriptLexAmount; i++) {
		if (scriptObjects[i].id == id) {
			lex = (ScriptLexPtrC)&scriptObjects[i];
		}
	}

	if (lines) {
		lex->line = lines;
	}

	return lex;
}

ScriptLexPtrC ScriptParser::Find(ScriptLexPtr lex, StringC word) {

	Uint32 lines = lex ? lex->line : 0;

	lex = LAME_NULL;

	for (Uint32 i = 0; i < kScriptLexAmount; i++) {
		if (scriptObjects[i].word == word) {
			lex = (ScriptLexPtrC)&scriptObjects[i];
		}
	}

	if (lines) {
		lex->line = lines;
	}

	return lex;
}

ScriptLexPtrC ScriptParser::Find(ScriptLexPtr lex, BufferRefC word) {

	Uint32 lines = lex ? lex->line : 0;

	lex = LAME_NULL;

	for (Uint32 i = 0; i < kScriptLexAmount; i++) {
		if (scriptObjects[i].word == word) {
			lex = (ScriptLexPtrC)&scriptObjects[i];
		}
	}

	if (lines) {
		lex->line = lines;
	}

	return lex;
}

ScriptLexPtrC ScriptParser::_ParseLex(StringC* wordPtr, Buffer* name) {

	static StringC lockedTokens = "\n\r\t ";
	static StringC allowedTokens = "@$_=/|&><+-*";
	static StringC wordTokens = "_$";

	StringC savedWord = *wordPtr;
	StringC word = *wordPtr;
	Buffer lexWord;
	ScriptLexPtr lex = LAME_NULL;
	Bool isWord;

	if (*word == '\"' || *word == '\'') {
		while (*++word != *savedWord) {
			if (!*word) {
				PostSyntaxError(0, "Quote or Apostrophe mismatched", 1);
			}
			lexWord.append(1, *word);
		}
		++word;
		lex = (ScriptLexPtr)this->Find(lex, kScriptLexString);
		goto __Return;
	}

	if (IsFloatValue(word)) {
		lex = this->Find(lex, kScriptLexFloat);
		strtof(word, (String*)&word);
		if (*word == 'f') {
			++word;
		}
		goto __SaveWord;
	}
	else if (IsHexValue(word)) {
		lex = this->Find(lex, kScriptLexInt);
		strtol(word + 2, (String*)&word, 16);
		goto __SaveWord;
	}
	else if (IsIntValue(word)) {
		lex = this->Find(lex, kScriptLexInt);
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
		lex = this->Find(lex, lexWord);
		if (!lex) {
			lex = this->Find(lex, kScriptLexDefault);
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

	return ScriptLexPtrC(lex);
}

ScriptParser::ScriptParser() {

	for (ScriptLexNodePtr lex : this->lexList_) {
		delete lex;
	}
	this->lexList_.clear();
}

ScriptParser::~ScriptParser() {

	for (ScriptLexNodePtr lex : this->lexList_) {
		delete lex;
	}
	this->lexList_.clear();
}

LAME_END