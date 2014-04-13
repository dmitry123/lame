#include "Script.h"

LAME_BEGIN

#define P(_p) Uint32(20-_p)

#define DA kScriptAssociativityDefault
#define LA kScriptAssociativityLeft
#define RA kScriptAssociativityRight

namespace internal {
	static const ScriptObject scriptObjects[kScriptObjectAmount] =
	{
		{ "${name}", P(16), kScriptObjectVariable, 1, LA },
		{ "${int}", P(16), kScriptObjectInt, 1, RA },
		{ "${float}", P(16), kScriptObjectFloat, 1, RA },
		{ "${string}", P(16), kScriptObjectString, 1, RA },
		{ "${bool}", P(16), kScriptObjectBool, 1, RA },
		{ "[", P(16), kScriptObjectBracketL, 1, DA },
		{ "]", P(16), kScriptObjectBracketR, 1, DA },
		{ "(", P(0), kScriptObjectParentheseL, 1, DA },
		{ ")", P(0), kScriptObjectParentheseR, 1, DA },
		{ ".", P(16), kScriptObjectDirect, 1, DA },
		{ "->", P(16), kScriptObjectMediated, 1, DA },
		{ "++", P(15), kScriptObjectIncrement, 1, LA },
		{ "--", P(15), kScriptObjectDecrement, 1, LA },
		{ "sizeof", P(15), kScriptObjectSizeof, 1, RA },
		{ "~", P(15), kScriptObjectBitNot, 1, RA },
		{ "!", P(15), kScriptObjectNot, 1, RA },
		{ "-", P(15), kScriptObjectUnaryMinus, 1, RA },
		{ "+", P(15), kScriptObjectUnaryPlus, 1, RA },
		{ "${type}", P(15), kScriptObjectType, 1, RA },
		{ "if", P(14), kScriptObjectIf, 1, RA },
		{ "else", P(14), kScriptObjectElse, 0, RA },
		{ "while", P(14), kScriptObjectWhile, 1, RA },
		{ "do", P(14), kScriptObjectDo, 0, RA },
		{ "for", P(14), kScriptObjectFor, 1, RA },
		{ "${function}", P(14), kScriptObjectFunction, 0, RA },
		{ "*", P(13), kScriptObjectMul, 2, RA },
		{ "/", P(13), kScriptObjectDiv, 2, RA },
		{ "%", P(13), kScriptObjectMod, 2, RA },
		{ "+", P(12), kScriptObjectAdd, 2, RA },
		{ "-", P(12), kScriptObjectSub, 2, RA },
		{ "<<", P(11), kScriptObjectBitShiftL, 2, RA },
		{ ">>", P(11), kScriptObjectBitShiftR, 2, RA },
		{ "<", P(10), kScriptObjectBellow, 2, RA },
		{ ">", P(10), kScriptObjectAbove, 2, RA },
		{ "<=", P(10), kScriptObjectBellowEqual, 2, RA },
		{ ">=", P(10), kScriptObjectAboveEqual, 2, RA },
		{ "==", P(9), kScriptObjectEqual, 2, RA },
		{ "!=", P(9), kScriptObjectNotEqual, 2, RA },
		{ "&", P(8), kScriptObjectBitAnd, 2, RA },
		{ "^", P(7), kScriptObjectBitXor, 2, RA },
		{ "|", P(6), kScriptObjectBitOr, 2, RA },
		{ "&&", P(5), kScriptObjectAnd, 2, RA },
		{ "||", P(4), kScriptObjectOr, 2, RA },
		{ "=", P(2), kScriptObjectSet, 2, LA },
		{ "+=", P(2), kScriptObjectAddSet, 2, LA },
		{ "-=", P(2), kScriptObjectSubSet, 2, LA },
		{ "*=", P(2), kScriptObjectMulSet, 2, LA },
		{ "/=", P(2), kScriptObjectDivSet, 2, LA },
		{ "%=", P(2), kScriptObjectModSet, 2, LA },
		{ "<<=", P(2), kScriptObjectBitShiftSetL, 2, LA },
		{ ">>=", P(2), kScriptObjectBitShiftSetR, 2, LA },
		{ "&=", P(2), kScriptObjectBitAndSet, 2, LA },
		{ "^=", P(2), kScriptObjectBitXorSet, 2, LA },
		{ "|=", P(2), kScriptObjectBitOrSet, 2, LA },
		{ ",", P(1), kScriptObjectComa, 2, RA },
		{ "/*", P(1), kScriptObjectCommentL, 0, DA },
		{ "*/", P(1), kScriptObjectCommentR, 0, DA },
		{ "//", P(1), kScriptObjectLineComment, 0, DA },
		{ "\"", P(1), kScriptObjectQuote, 0, DA },
		{ "\'", P(1), kScriptObjectApostrophe, 0, DA },
		{ ";", P(1), kScriptObjectSemicolon, 0, DA },
		{ "{", P(1), kScriptObjectBraceL, 0, DA },
		{ "}", P(1), kScriptObjectBraceR, 0, DA },
		{ "class", P(1), kScriptObjectClass, 0, DA },
		{ "abstract", P(1), kScriptObjectAbstract, 0, DA },
		{ "implements", P(1), kScriptObjectImplements, 0, DA },
		{ "extends", P(1), kScriptObjectExtends, 0, DA },
		{ "public", P(1), kScriptObjectPublic, 0, DA },
		{ "private", P(1), kScriptObjectPrivate, 0, DA },
		{ "protected", P(1), kScriptObjectProtected, 0, DA },
		{ "final", P(1), kScriptObjectFinal, 0, DA },
		{ "override", P(1), kScriptObjectOverride, 0, DA },
		{ "static", P(1), kScriptObjectStatic, 0, DA },
		{ "break", P(1), kScriptObjectBreak, 0, DA },
		{ "continue", P(1), kScriptObjectContinue, 0, DA },
		{ "try", P(1), kScriptObjectTry, 0, DA },
		{ "catch", P(1), kScriptObjectCatch, 0, DA },
		{ "throw", P(1), kScriptObjectThrow, 0, DA },
		{ "thread", P(1), kScriptObjectThread, 0, DA }
	};
}

Void ScriptObject::Parse(StringC* word) {

	// tokens which parser ignores
	static StringC lockedTokens = "\n\r\t :";
	// tokens which parser allow
	static StringC allowedTokens = "@$_";
	// is array with objects was sorted
	static Bool isSorted = LAME_FALSE;

	if (!isSorted) {
		// sort list to avoid problems with object's words length
		qsort((VoidP)internal::scriptObjects, kScriptObjectAmount, sizeof(ScriptObject), [](
			const void* left,
			const void* right
		) -> int {
			return ((ScriptObjectPtr)right)->word.length() - ((ScriptObjectPtr)left)->word.length();
		});
		// change flag to sorted
		isSorted = LAME_TRUE;
	}

	// reset self
	this->Reset();

	// save current position
	StringC savedWord = *word;

	// checking word for float value
	if (IsFloatValue(*word)) {
		// save object type
		this->object = kScriptObjectFloat;
		this->type = kScriptTypeFloat;
		// move word to the next position after value
		strtof(*word, (String*)word);
		if (**word == 'f') {
			++(*word);
		}
		// go and save word
		goto __SaveWord;
	}
	// checking word for hex value
	else if (IsHexValue(*word)) {
		// save object type
		this->object = kScriptObjectInt;
		this->type = kScriptTypeInt;
		// move word to next position
		strtol(*(word)+2, (String*)word, 16);
		// go and save word
		goto __SaveWord;
	}
	// checking for int value
	else if (IsIntValue(*word)) {
		// save object type
		this->object = kScriptObjectInt;
		this->type = kScriptTypeInt;
		// move pointer
		strtol(*word, (String*)word, 10);
		// save word
		goto __SaveWord;
	}
	// checking token for ignore
	else if (strchr(lockedTokens, **word)) {
		// goto next position
		++(*word);
		// exit from function
		goto __Return;
	}
	// seeking word in object list
	else {
		for (Uint32 i = 0; i < kScriptObjectAmount; i++) {
			if (!strncmp(
				// compare object's data
				internal::scriptObjects[i].word.data(),
				// with current word
				*word,
				// using internal length
				strlen(internal::scriptObjects[i].word.data()))
			) {
				// copy object to self
				*this = internal::scriptObjects[i];
				// move word's pointer
				(*word) += this->word.length();
				// exit from function
				goto __Return;
			}
		}
	}

	// avoiding letters, digits and allowed tokens (we've found variable, function or type)
	while (IsDigit(**word) || IsLetter(**word) || strchr(allowedTokens, **word)) {
		++(*word);
	}

	// save word if its length above then 0
__SaveWord:
	if (!this->word.length()) {
		this->word.append((String)savedWord, *word - savedWord);
	}

	// exit from function
__Return:;
}

StringC ScriptObject::GetString(Void) const {
	// return object's name
	return this->word.data();
}

Void ScriptObject::Reset(Void) {
	// clear all fields
	this->word.clear();
	this->priority = 0;
	this->object = kScriptObjectDefault;
	this->arguments = 0;
	this->associativity = DA;
	this->line = 0;
	this->type = kScriptTypeDefault;
}

Bool ScriptObject::IsDigit(Sint8 symbol) {
	// cheking for digit
	return symbol >= '0' && symbol <= '9';
}

Bool ScriptObject::IsLetter(Sint8 symbol) {
	// checking for letter
	return (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z');
}

Bool ScriptObject::IsIntValue(StringC string) {
	String result;
	// parsing word
	strtoll(string, &result, 10);
	// checking for int
	return string != result && isdigit(*string);
}

Bool ScriptObject::IsHexValue(StringC string) {
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

Bool ScriptObject::IsStringValue(StringC string) {
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

Bool ScriptObject::IsIntValue(const Buffer& string) {
	return IsIntValue(string.data());
}

Bool ScriptObject::IsFloatValue(StringC string) {
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

Bool ScriptObject::IsFloatValue(const Buffer& string) {
	// checking for length
	if (!string.length()) {
		return LAME_FALSE;
	}
	else {
		// parsing float
		return IsFloatValue(string.data());
	}
}

ScriptNativeInt ScriptObject::ParseIntValue(StringC string) {
	// if value hex then parse as hex number
	if (IsHexValue(string)) {
		return strtoll(string, NULL, 16);
	}
	// else parse as normal number
	return strtoll(string, NULL, 10);
}

ScriptNativeFloat ScriptObject::ParseFloatValue(StringC string) {
	// parsing float value
	return strtof(string, NULL);
}

ScriptNativeString ScriptObject::ParseStringValue(StringC string) {
    return string;
}

ScriptObjectPtr ScriptObject::FindScriptObjectByFlag(EScriptObject object) {
	for (Uint32 i = 0; i < kScriptObjectAmount; i++) {
		if (internal::scriptObjects[i].object == object) {
			return (ScriptObjectPtr)&internal::scriptObjects[i];
		}
	}
	return LAME_NULL;
}

LAME_END