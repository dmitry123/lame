#include "Script.h"
#include "Internal.h"

LAME_BEGIN

#define P(_p) Uint32(20-_p)

#define DA kScriptAssociativityDefault
#define LA kScriptAssociativityLeft
#define RA kScriptAssociativityRight

#define ABS(_value) ((_value) < 0 ? -(_value) : (_value))

namespace internal {
	static const ScriptLex scriptObjects[] =
	{
		/* Script special objects */
		{ "${variable}", P(15), kScriptObjectVariable, 1, LA },
		{ "${int}", P(15), kScriptObjectInt, 1, RA },
		{ "${float}", P(15), kScriptObjectFloat, 1, RA },
		{ "${string}", P(15), kScriptObjectString, 1, RA },
		{ "${bool}", P(15), kScriptObjectBool, 1, RA },
		{ "${type}", P(17), kScriptObjectType, 0, RA },
		{ "${function}", P(15), kScriptObjectFunction, 0, LA },
        
		/* Script language objects */
		{ "implements", P(1), kScriptObjectImplements, 0, DA },
		{ "continue", P(1), kScriptObjectContinue, 0, DA },
		{ "extends", P(1), kScriptObjectExtends, 0, DA },
		{ "sizeof", P(15), kScriptObjectSizeof, 1, RA },
		{ "class", P(1), kScriptObjectClass, 0, DA },
		{ "break", P(1), kScriptObjectBreak, 0, DA },
		{ "catch", P(1), kScriptObjectCatch, 0, DA },
		{ "throw", P(1), kScriptObjectThrow, 0, DA },
		{ "while", P(14), kScriptObjectWhile, 1, RA },
		{ "else", P(14), kScriptObjectElse, 0, RA },
		{ "try", P(1), kScriptObjectTry, 0, DA },
		{ "new", P(20), kScriptObjectNew, 1, RA },
		{ "for", P(14), kScriptObjectFor, 1, RA },
		{ "do", P(14), kScriptObjectDo, 0, RA },
		{ "if", P(14), kScriptObjectIf, 1, RA },
		{ "<<=", P(2), kScriptObjectBitShiftSetL, 2, LA },
		{ ">>=", P(2), kScriptObjectBitShiftSetR, 2, LA },
		{ "->", P(16), kScriptObjectMediated, 2, RA },
		{ "++", P(15), kScriptObjectIncrement, 1, LA },
		{ "--", P(15), kScriptObjectDecrement, 1, LA },
		{ "<<", P(11), kScriptObjectBitShiftL, 2, RA },
		{ ">>", P(11), kScriptObjectBitShiftR, 2, RA },
		{ "<=", P(10), kScriptObjectBellowEqual, 2, RA },
		{ ">=", P(10), kScriptObjectAboveEqual, 2, RA },
		{ "==", P(9), kScriptObjectEqual, 2, RA },
		{ "!=", P(9), kScriptObjectNotEqual, 2, RA },
		{ "&&", P(5), kScriptObjectAnd, 2, RA },
		{ "||", P(4), kScriptObjectOr, 2, RA },
		{ "=", P(2), kScriptObjectSet, 2, LA },
		{ "+=", P(2), kScriptObjectAddSet, 2, LA },
		{ "-=", P(2), kScriptObjectSubSet, 2, LA },
		{ "*=", P(2), kScriptObjectMulSet, 2, LA },
		{ "/=", P(2), kScriptObjectDivSet, 2, LA },
		{ "%=", P(2), kScriptObjectModSet, 2, LA },
		{ "&=", P(2), kScriptObjectBitAndSet, 2, LA },
		{ "^=", P(2), kScriptObjectBitXorSet, 2, LA },
		{ "|=", P(2), kScriptObjectBitOrSet, 2, LA },
		{ "/*", P(1), kScriptObjectCommentL, 0, DA },
		{ "*/", P(1), kScriptObjectCommentR, 0, DA },
		{ "//", P(1), kScriptObjectLineComment, 0, DA },
		{ ",", P(1), kScriptObjectComa, 2, RA },
		{ "[", P(16), kScriptObjectBracketL, 1, DA },
		{ "]", P(16), kScriptObjectBracketR, 1, DA },
		{ "(", P(0), kScriptObjectParentheseL, 1, DA },
		{ ")", P(0), kScriptObjectParentheseR, 1, DA },
		{ ".", P(16), kScriptObjectDirect, 2, RA },
		{ "~", P(15), kScriptObjectBitNot, 1, RA },
		{ "!", P(15), kScriptObjectNot, 1, RA },
		{ "±", P(15), kScriptObjectUnaryMinus, 1, RA },
		{ "±", P(15), kScriptObjectUnaryPlus, 1, RA },
		{ "*", P(13), kScriptObjectMul, 2, RA },
		{ "/", P(13), kScriptObjectDiv, 2, RA },
		{ "%", P(13), kScriptObjectMod, 2, RA },
		{ "+", P(12), kScriptObjectAdd, 2, RA },
		{ "-", P(12), kScriptObjectSub, 2, RA },
		{ "<", P(10), kScriptObjectBellow, 2, RA },
		{ ">", P(10), kScriptObjectAbove, 2, RA },
		{ "&", P(8), kScriptObjectBitAnd, 2, RA },
		{ "^", P(7), kScriptObjectBitXor, 2, RA },
		{ "|", P(6), kScriptObjectBitOr, 2, RA },
		{ "\"", P(1), kScriptObjectQuote, 0, DA },
		{ "\'", P(1), kScriptObjectApostrophe, 0, DA },
		{ ";", P(1), kScriptObjectSemicolon, 0, DA },
		{ "{", P(1), kScriptObjectBraceL, 0, DA },
		{ "}", P(1), kScriptObjectBraceR, 0, DA }
	};
}

Void ScriptLex::Parse(StringC* word) {
	*word = this->Parse(*word);
}

StringC ScriptLex::Parse(StringC word) {

	static StringC lockedTokens = "\n\r\t :";
	static StringC allowedTokens = "@$_=/|&><+-*";
	static StringC wordTokens = "_$";

	// save current position
	StringC savedWord = word;
    
	// reset self
	this->Reset();

	// checking word for float value
	if (IsFloatValue(word)) {
		// save object type
		this->object = kScriptObjectFloat;
		// move word to the next position after value
		strtof(word, (String*)&word);
		if (*word == 'f') {
			++word;
		}
		// go and save word
		goto __SaveWord;
	}
	// checking word for hex value
	else if (IsHexValue(word)) {
		// save object type
		this->object = kScriptObjectInt;
		// move word to next position
		strtol(word+2, (String*)&word, 16);
		// go and save word
		goto __SaveWord;
	}
	// checking for int value
	else if (IsIntValue(word)) {
		// save object type
		this->object = kScriptObjectInt;
		// move pointer
		strtol(word, (String*)&word, 10);
		// save word
		goto __SaveWord;
	}
	// checking token for ignore
	else if (strchr(lockedTokens, *word)) {
		// goto next position
		++word;
		// exit from function
		goto __Return;
	}
	// seeking word in object list
	else {
        // if word's firsty symbol is letter
        Bool isWord = IsLetter(*word);
        // find other symbols
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
        // copy result to string
        this->word.append(savedWord, Sint32(word - savedWord));
        // find result string in array
		for (Uint32 i = 0; i < kScriptObjectAmount; i++) {
			if (internal::scriptObjects[i].word == this->word) {
				// copy object to self
				*this = internal::scriptObjects[i];
				// exit from function
				goto __Return;
			}
		}
	}

	// save word if its length above then 0
__SaveWord:
	if (!this->word.length()) {
		this->word.append((String)savedWord, word - savedWord);
	}

	// exit from function
__Return:;
	return word;
}

Void ScriptLex::Reset(Void) {

	// reset all fields
	this->word.clear();
	this->priority = 0;
	this->object = kScriptObjectDefault;
	this->arguments = 0;
	this->associativity = kScriptAssociativityDefault;
	this->line = 0;
}

ScriptLexPtr ScriptLex::Find(EScriptObject object) {

	// find and return lex object
	for (Uint32 i = 0; i < kScriptObjectAmount; i++) {
		if (internal::scriptObjects[i].object == object) {
			return (ScriptLexPtr)&internal::scriptObjects[i];
		}
	}
	return LAME_NULL;
}

LAME_END