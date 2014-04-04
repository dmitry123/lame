#include "Script.h"

#include <algorithm>

LAME_BEGIN

static inline Bool IsDigit(Sint8 symbol) {

	return
		symbol >= '0' &&
		symbol <= '9';
}

static inline Bool IsLetter(Sint8 symbol) {

	return
		(symbol >= 'a' && symbol <= 'z') ||
		(symbol >= 'A' && symbol <= 'Z');
}

static inline Bool IsIntValue(StringC string) {

	String result;

	strtol(string, &result, 10);

	if (string != result && isdigit(*string)) {
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

	Sint8P finish;

	if (!string) {
		return LAME_FALSE;
	}

	strtof(string, &finish);

	if (finish != string && (*finish == 'f' || !*finish)) {
		return LAME_TRUE;
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

#define P(_p) 16-_p

namespace internal {
	static const ScriptObject scriptObjects[kScriptAmount] = {
		/* FIRST */
		{ "${name}", P(0), kScriptVariable, kScriptArgDefault, kScriptAssociativityDefault },
		{ "${int}", P(0), kScriptInt, kScriptArgDefault, kScriptAssociativityDefault },
		{ "${float}", P(0), kScriptFloat, kScriptArgDefault, kScriptAssociativityDefault },
		{ "${string}", P(0), kScriptString, kScriptArgDefault, kScriptAssociativityDefault },
		/* POSTFIX */
		{ "[", P(16), kScriptBracketL, kScriptArgPostfix, kScriptAssociativityRight },
		{ "]", P(16), kScriptBracketR, kScriptArgPostfix, kScriptAssociativityRight },
		{ "(", P(17), kScriptParentheseL, kScriptArgPostfix, kScriptAssociativityRight },
		{ ")", P(17), kScriptParentheseR, kScriptArgPostfix, kScriptAssociativityRight },
		{ ".", P(16), kScriptDirectSelection, kScriptArgPostfix, kScriptAssociativityRight },
		{ "->", P(16), kScriptMediatedSelection, kScriptArgPostfix, kScriptAssociativityRight },
		{ "${declare}", P(15), kScriptDeclare, kScriptArgPostfix, kScriptAssociativityLeft },
		{ "++", P(15), kScriptIncrement, kScriptArgPostfix, kScriptAssociativityLeft },
		{ "--", P(15), kScriptDecrement, kScriptArgPostfix, kScriptAssociativityLeft },
		/* UNARY */
		{ "sizeof", P(15), kScriptSizeof, kScriptArgUnary, kScriptAssociativityLeft },
		{ "~", P(15), kScriptBitNot, kScriptArgUnary, kScriptAssociativityLeft },
		{ "!", P(15), kScriptNot, kScriptArgUnary, kScriptAssociativityLeft },
		{ "-", P(15), kScriptUnaryMinus, kScriptArgUnary, kScriptAssociativityLeft },
		{ "+", P(15), kScriptUnaryPlus, kScriptArgUnary, kScriptAssociativityLeft },
		{ "&", P(15), kScriptOffset, kScriptArgUnary, kScriptAssociativityLeft },
		{ "*", P(15), kScriptClaim, kScriptArgUnary, kScriptAssociativityLeft },
		{ "${type}", P(15), kScriptType, kScriptArgUnary, kScriptAssociativityLeft },
		{ "if", P(14), kScriptIf, kScriptArgUnary, kScriptAssociativityRight },
		{ "else", P(14), kScriptElse, kScriptArgDefault, kScriptAssociativityRight },
		{ "while", P(14), kScriptWhile, kScriptArgUnary, kScriptAssociativityRight },
		{ "do", P(14), kScriptDo, kScriptArgDefault, kScriptAssociativityRight },
		{ "for", P(14), kScriptFor, kScriptArgUnary, kScriptAssociativityRight },
		/* BINARY */
		{ "*", P(13), kScriptMul, kScriptArgBinary, kScriptAssociativityRight },
		{ "/", P(13), kScriptDiv, kScriptArgBinary, kScriptAssociativityRight },
		{ "%", P(13), kScriptMod, kScriptArgBinary, kScriptAssociativityRight },
		{ "+", P(12), kScriptAdd, kScriptArgBinary, kScriptAssociativityRight },
		{ "-", P(12), kScriptSub, kScriptArgBinary, kScriptAssociativityRight },
		{ "<<", P(11), kScriptBitShiftL, kScriptArgBinary, kScriptAssociativityRight },
		{ ">>", P(11), kScriptBitShiftR, kScriptArgBinary, kScriptAssociativityRight },
		{ "<", P(10), kScriptBellow, kScriptArgBinary, kScriptAssociativityRight },
		{ ">", P(10), kScriptAbove, kScriptArgBinary, kScriptAssociativityRight },
		{ "<=", P(10), kScriptBellowEqual, kScriptArgBinary, kScriptAssociativityRight },
		{ ">=", P(10), kScriptAboveEqual, kScriptArgBinary, kScriptAssociativityRight },
		{ "==", P(9), kScriptEqual, kScriptArgBinary, kScriptAssociativityRight },
		{ "!=", P(9), kScriptNotEqual, kScriptArgBinary, kScriptAssociativityRight },
		{ "&", P(8), kScriptBitAnd, kScriptArgBinary, kScriptAssociativityRight },
		{ "^", P(7), kScriptBitXor, kScriptArgBinary, kScriptAssociativityRight },
		{ "|", P(6), kScriptBitOr, kScriptArgBinary, kScriptAssociativityRight },
		{ "&&", P(5), kScriptAnd, kScriptArgBinary, kScriptAssociativityRight },
		{ "||", P(4), kScriptOr, kScriptArgBinary, kScriptAssociativityRight },
		{ "=", P(2), kScriptSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "+=", P(2), kScriptAddSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "-=", P(2), kScriptSubSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "*=", P(2), kScriptMulSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "/=", P(2), kScriptDivSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "%=", P(2), kScriptModSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "<<=", P(2), kScriptBitShiftSetL, kScriptArgBinary, kScriptAssociativityLeft },
		{ ">>=", P(2), kScriptBitShiftSetR, kScriptArgBinary, kScriptAssociativityLeft },
		{ "&=", P(2), kScriptBitAndSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "^=", P(2), kScriptBitXorSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ "|=", P(2), kScriptBitOrSet, kScriptArgBinary, kScriptAssociativityLeft },
		{ ",", P(1), kScriptComa, kScriptArgBinary, kScriptAssociativityRight },
		/* OTHER */
		{ "/*", P(1), kScriptCommentL, kScriptArgDefault, kScriptAssociativityDefault },
		{ "*/", P(1), kScriptCommentR, kScriptArgDefault, kScriptAssociativityDefault },
		{ "//", P(1), kScriptCommentLine, kScriptArgDefault, kScriptAssociativityDefault },
		{ "\"", P(1), kScriptQuote, kScriptArgDefault, kScriptAssociativityDefault },
		{ "\'", P(1), kScriptApostrophe, kScriptArgDefault, kScriptAssociativityDefault },
		{ ";", P(1), kScriptSemicolon, kScriptArgDefault, kScriptAssociativityDefault },
		{ "{", P(1), kScriptBraceL, kScriptArgDefault, kScriptAssociativityDefault },
		{ "}", P(1), kScriptBraceR, kScriptArgDefault, kScriptAssociativityDefault }
	};
}

ScriptObject& ScriptObject::Parse(StringC* word) {

	static StringC tokens = "\n\r\t :@$";
	static Bool isSorted = LAME_FALSE;

	if (!isSorted) {
		std::qsort((VoidP)internal::scriptObjects, kScriptAmount, sizeof(ScriptObject), [](
			const void* left,
			const void* right
		) -> int {
			return ((ScriptObjectPtr)right)->word.length() - ((ScriptObjectPtr)left)->word.length();
		});
		isSorted = LAME_TRUE;
	}

	StringC savedWord = *word;

	this->args = kScriptArgDefault;
	this->flag = kScriptDefault;
	this->associativity = kScriptAssociativityDefault;
	this->priority = 0;
    this->var = 0;
    this->line = 0;

	if (IsFloatValue(*word)) {
		this->flag = kScriptFloat;
		strtof(*word, (String*)word);
		if (**word == 'f') {
			++(*word);
		}
		goto __SaveWord;
	}
	else if (IsIntValue(*word)) {
		this->flag = kScriptInt;
		strtol(*word, (String*)word, 10);
		goto __SaveWord;
	}
	else if (strchr(tokens, **word)) {
		++(*word); return *this;
	}
	else {
		for (Uint32 i = 0; i < kScriptAmount; i++) {

			if (!strncmp(
				internal::scriptObjects[i].word.data(), *word,
				strlen(internal::scriptObjects[i].word.data()))
			) {
				*this = internal::scriptObjects[i];
				(*word) += this->word.length();
				goto __Return;
			}
		}
	}

	while (IsDigit(**word) || IsLetter(**word) || **word == '_') {
		++(*word);
	}

__SaveWord:
	if (!this->word.length()) {
		this->word.append((String)savedWord, *word - savedWord);
	}

__Return:
	return *this;
}

StringC ScriptObject::GetString(Void) const {
	return internal::scriptObjects[this->flag].word.data();
}


LAME_END