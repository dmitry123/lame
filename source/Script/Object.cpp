#include "Script.h"
#include "Internal.h"

#include <algorithm>

LAME_BEGIN

#define P(_p) Uint32(16-_p)

namespace internal {
	static const ScriptObject scriptObjects[kScriptAmount] = {
		/* FIRST */
		{ "${name}", P(15), kScriptVariable, kScriptArgUnary, kScriptAssociativityDefault },
		{ "${int}", P(14), kScriptInt, kScriptArgUnary, kScriptAssociativityDefault },
		{ "${float}", P(14), kScriptFloat, kScriptArgUnary, kScriptAssociativityDefault },
		{ "${string}", P(14), kScriptString, kScriptArgUnary, kScriptAssociativityDefault },
		/* POSTFIX */
		{ "[", P(16), kScriptBracketL, kScriptArgPostfix, kScriptAssociativityRight },
		{ "]", P(16), kScriptBracketR, kScriptArgPostfix, kScriptAssociativityRight },
		{ "(", P(17), kScriptParentheseL, kScriptArgPostfix, kScriptAssociativityRight },
		{ ")", P(17), kScriptParentheseR, kScriptArgPostfix, kScriptAssociativityRight },
		{ ".", P(16), kScriptDirectSelection, kScriptArgPostfix, kScriptAssociativityRight },
		{ "->", P(16), kScriptMediatedSelection, kScriptArgPostfix, kScriptAssociativityRight },
		{ "${declare}", P(15), kScriptDeclare, kScriptArgUnary, kScriptAssociativityLeft },
		{ "++", P(15), kScriptIncrement, kScriptArgUnary, kScriptAssociativityLeft },
		{ "--", P(15), kScriptDecrement, kScriptArgUnary, kScriptAssociativityLeft },
		/* UNARY */
		{ "sizeof", P(15), kScriptSizeof, kScriptArgUnary, kScriptAssociativityRight },
		{ "~", P(15), kScriptBitNot, kScriptArgUnary, kScriptAssociativityRight },
		{ "!", P(15), kScriptNot, kScriptArgUnary, kScriptAssociativityRight },
//		{ "-", P(15), kScriptUnaryMinus, kScriptArgUnary, kScriptAssociativityRight },
//		{ "+", P(15), kScriptUnaryPlus, kScriptArgUnary, kScriptAssociativityRight },
//		{ "&", P(15), kScriptOffset, kScriptArgUnary, kScriptAssociativityRight },
//		{ "*", P(15), kScriptClaim, kScriptArgUnary, kScriptAssociativityRight },
		{ "${type}", P(15), kScriptType, kScriptArgUnary, kScriptAssociativityRight },
		{ "if", P(14), kScriptIf, kScriptArgUnary, kScriptAssociativityRight },
		{ "else", P(14), kScriptElse, kScriptArgDefault, kScriptAssociativityRight },
		{ "while", P(14), kScriptWhile, kScriptArgUnary, kScriptAssociativityRight },
		{ "do", P(14), kScriptDo, kScriptArgDefault, kScriptAssociativityRight },
		{ "for", P(14), kScriptFor, kScriptArgUnary, kScriptAssociativityRight },
		{ "function", P(14), kScriptFunction, kScriptArgUnary, kScriptAssociativityRight },
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
		qsort((VoidP)internal::scriptObjects, kScriptAmount, sizeof(ScriptObject), [](
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
	this->type = 0;

	if (IsFloatValue(*word)) {
		this->flag = kScriptFloat;
		strtof(*word, (String*)word);
		if (**word == 'f') {
			++(*word);
		}
		goto __SaveWord;
	}
	else if (IsHexValue(*word)) {
		this->flag = kScriptInt;
		strtol(*(word) + 2, (String*)word, 16);
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

Void ScriptObject::Reset(Void) {

	this->priority = 0;
	this->line = 0;
	this->var = 0;
	this->type = 0;
	this->flag = kScriptDefault;
	this->args = kScriptArgDefault;
	this->associativity = kScriptAssociativityDefault;
}

ScriptObjectPtr ScriptObject::FindScriptObjectByFlag(ScriptFlag flag) {

	for (Uint32 i = 0; i < kScriptAmount; i++) {
		if (internal::scriptObjects[i].flag == flag) {
			return (ScriptObjectPtr)&internal::scriptObjects[i];
		}
	}
	return LAME_NULL;
}

LAME_END