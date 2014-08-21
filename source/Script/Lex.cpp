#include "Lex.h"

LAME_BEGIN2(Script)

#define P(_priority, _reverse) _reverse ? Uint32(18 - _priority) : Uint32(_priority)

Map<LexID, Lex> Lex::lexMap = {

		/* Default */

		{ kScriptLexDefault, {
			"#", P(0, 1), kScriptLexDefault, 0,
				kScriptLexFlagRight | kScriptLexFlagUnknown }
		},

		/* Constants */

		{ kScriptLexInt, {
			"#", P(0, 1), kScriptLexInt, 0,
				kScriptLexFlagRight | kScriptLexFlagConst }
		},
		{ kScriptLexFloat, {
			"#", P(0, 1), kScriptLexFloat, 0,
				kScriptLexFlagRight | kScriptLexFlagConst }
		},
		{ kScriptLexString, {
			"#", P(0, 1), kScriptLexString, 0,
				kScriptLexFlagRight | kScriptLexFlagConst }
		},
		{ kScriptLexCharacter, {
			"#", P(0, 1), kScriptLexCharacter, 0,
				kScriptLexFlagRight | kScriptLexFlagConst }
		},

		/* Brackets */

		{ kScriptLexBracketL,
			{ "[", P(2, 0), kScriptLexBracketL, 0,
				kScriptLexFlagRight }
		},
		{ kScriptLexBracketR,
			{ "]", P(2, 0), kScriptLexBracketR, 0,
				kScriptLexFlagRight }
		},
		{ kScriptLexParenthesisL,
			{ "(", P(2, 0), kScriptLexParenthesisL, 0,
				kScriptLexFlagRight }
		},
		{ kScriptLexParenthesisR,
			{ ")", P(2, 0), kScriptLexParenthesisR, 0,
				kScriptLexFlagRight }
		},

		/* Specials */

		{ kScriptLexCast, {
			"#", P(3, 1), kScriptLexCast, 0,
				kScriptLexFlagRight | kScriptLexFlagUnknown }
		},
		{ kScriptLexArray,
			{ "[]", P(0, 0), kScriptLexArray, 0,
				kScriptLexFlagRight }
		},
		{ kScriptLexArgumentList,
			{ "...", P(0, 0), kScriptLexArgumentList, 0,
				kScriptLexFlagRight }
		},

		/* Selection */

		{ kScriptLexDirected,
			{ ".", P(2, 0), kScriptLexDirected, 2,
				kScriptLexFlagRight }
		},

		/* Unary Operators */

		{ kScriptLexIncrement,
			{ "++", P(2, 0), kScriptLexIncrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexDecrement,
			{ "--", P(2, 0), kScriptLexDecrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexPrefixIncrement,
			{ "++", P(3, 1), kScriptLexPrefixIncrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexPrefixDecrement,
			{ "--", P(3, 1), kScriptLexPrefixDecrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexInstanceof,
			{ "instanceof", P(3, 0), kScriptLexInstanceof, 1,
				kScriptLexFlagRight }
		},
		{ kScriptLexReturn,
			{ "return", P(0, 0), kScriptLexReturn, 1,
				kScriptLexFlagLanguage | kScriptLexFlagRight }
		},
		{ kScriptLexBitNot,
			{ "~", P(3, 1), kScriptLexBitNot, 1,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexNot,
			{ "!", P(3, 1), kScriptLexNot, 1,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexUnaryMinus,
			{ "#", P(3, 1), kScriptLexUnaryMinus, 1,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexUnaryPlus,
			{ "#", P(3, 1), kScriptLexUnaryPlus, 1,
				kScriptLexFlagRight | kScriptLexFlagMath }	
		},
		{ kScriptLexNew,
			{ "new", P(3, 1), kScriptLexNew, 1,
				kScriptLexFlagRight | kScriptLexFlagLanguage }
		},
		
		/* Language Constructions */

		{ kScriptLexIf,
			{ "if", P(0, 1), kScriptLexIf, 1,
				kScriptLexFlagCondition | kScriptLexFlagLanguage }
		},
		{ kScriptLexElse,
			{ "else", P(0, 1), kScriptLexElse, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage | kScriptLexFlagWoParentheses }
		},
		{ kScriptLexWhile,
			{ "while", P(0, 1), kScriptLexWhile, 1,
				kScriptLexFlagCondition | kScriptLexFlagLanguage }
		},
		{ kScriptLexDo,
			{ "do", P(0, 1), kScriptLexDo, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage | kScriptLexFlagWoParentheses }
		},
		{ kScriptLexFor,
			{ "for", P(0, 1), kScriptLexFor, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage }
		},
		{ kScriptLexTry,
			{ "try", P(0, 1), kScriptLexTry, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage | kScriptLexFlagWoParentheses }
		},
		{ kScriptLexCatch,
			{ "catch", P(0, 1), kScriptLexCatch, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage }
		},
		{ kScriptLexFinally,
			{ "finally", P(0, 1), kScriptLexFinally, 0,
				kScriptLexFlagCondition | kScriptLexFlagLanguage | kScriptLexFlagWoParentheses }
		},

		/* Binary Operators */

		{ kScriptLexMul,
			{ "*", P(5, 1), kScriptLexMul, 2,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexDiv,
			{ "/", P(5, 1), kScriptLexDiv, 2,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexMod,
			{ "%", P(5, 1), kScriptLexMod, 2,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexAdd,
			{ "+", P(6, 1), kScriptLexAdd, 2,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexSub,
			{ "-", P(6, 1), kScriptLexSub, 2,
				kScriptLexFlagRight | kScriptLexFlagMath }
		},
		{ kScriptLexBitShiftL,
			{ "<<", P(7, 1), kScriptLexBitShiftL, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitShiftR,
			{ ">>", P(7, 1), kScriptLexBitShiftR, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitAnd,
			{ "&", P(10, 1), kScriptLexBitAnd, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitXor,
			{ "^", P(11, 1), kScriptLexBitXor, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitOr,
			{ "|", P(12, 1), kScriptLexBitOr, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexSet,
			{ "=", P(16, 1), kScriptLexSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexAddSet,
			{ "+=", P(16, 1), kScriptLexAddSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexSubSet,
			{ "-=", P(16, 1), kScriptLexSubSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexMulSet,
			{ "*=", P(16, 1), kScriptLexMulSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexDivSet,
			{ "/=", P(16, 1), kScriptLexDivSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexModSet,
			{ "%=", P(16, 1), kScriptLexModSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath }
		},
		{ kScriptLexBitShiftSetL,
			{ "<<=", P(16, 1), kScriptLexBitShiftSetL, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitShiftSetR,
			{ ">>=", P(16, 1), kScriptLexBitShiftSetR, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitAndSet,
			{ "&=", P(16, 1), kScriptLexBitAndSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitXorSet,
			{ "^=", P(16, 1), kScriptLexBitXorSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBitOrSet,
			{ "|=", P(16, 1), kScriptLexBitOrSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic }
		},
		{ kScriptLexBellow,
			{ "<", P(8, 1), kScriptLexBellow, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexAbove,
			{ ">", P(8, 1), kScriptLexAbove, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexBellowEqual,
			{ "<=", P(8, 1), kScriptLexBellowEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexAboveEqual,
			{ ">=", P(8, 1), kScriptLexAboveEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexEqual,
			{ "==", P(9, 1), kScriptLexEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexNotEqual,
			{ "!=", P(9, 1), kScriptLexNotEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexAnd,
			{ "&&", P(13, 1), kScriptLexAnd, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexOr,
			{ "||", P(14, 1), kScriptLexOr, 2,
				kScriptLexFlagRight | kScriptLexFlagBool }
		},
		{ kScriptLexTernary,
			{ "?", P(15, 1), kScriptLexTernary, 3,
				kScriptLexFlagDefault | kScriptLexFlagMath }
		},

		/* Others */

		{ kScriptLexComma,
			{ ",", P(18, 0), kScriptLexComma, 2,
				kScriptLexFlagRight }
		},
		{ kScriptLexCommentL,
			{ "/*", P(0, 0), kScriptLexCommentL, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexCommentR,
			{ "*/", P(0, 0), kScriptLexCommentR, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexLineComment,
			{ "//", P(0, 0), kScriptLexLineComment, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexQuote,
			{ "\"", P(0, 0), kScriptLexQuote, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexApostrophe,
			{ "\'", P(0, 0), kScriptLexApostrophe, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexSemicolon,
			{ ";", P(0, 0), kScriptLexSemicolon, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexColon,
			{ ":", P(0, 0), kScriptLexColon, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexBraceL,
			{ "{", P(0, 0), kScriptLexBraceL, 0,
				kScriptLexFlagDefault }
		},
		{ kScriptLexBraceR,
			{ "}", P(0, 0), kScriptLexBraceR, 0,
				kScriptLexFlagDefault }
		},

		/* Language Keywords */

		{ kScriptLexClass,
			{ "class", P(0, 0), kScriptLexClass, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexEnum,
			{ "enum", P(0, 1), kScriptLexEnum, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexInterface,
			{ "interface", P(0, 0), kScriptLexInterface, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexImplements,
			{ "implements", P(0, 0), kScriptLexImplements, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexExtends,
			{ "extends", P(0, 0), kScriptLexExtends, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexBreak,
			{ "break", P(0, 0), kScriptLexBreak, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexContinue,
			{ "continue", P(0, 0), kScriptLexContinue, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexPublic,
			{ "public", P(0, 0), kScriptLexPublic, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexPrivate,
			{ "private", P(0, 0), kScriptLexPrivate, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexProtected,
			{ "protected", P(0, 0), kScriptLexProtected, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexStatic,
			{ "static", P(0, 0), kScriptLexStatic, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexNative,
			{ "native", P(0, 0), kScriptLexNative, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexFinal,
			{ "final", P(0, 0), kScriptLexFinal, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexAbstract,
			{ "abstract", P(0, 0), kScriptLexAbstract, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexOverride,
			{ "@Override", P(0, 0), kScriptLexOverride, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexDecprecated,
			{ "@Deprecated", P(0, 0), kScriptLexDecprecated, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		}
};

LexPtrC Lex::Find(LexID id) {

	Map<LexID, Lex>::iterator i;

	if ((i = lexMap.find(id)) != lexMap.end()) {
		return &i->second;
	}

	return NULL;
}

LexPtrC Lex::Find(BufferRefC word) {

	for (auto& i : lexMap) {
		if (i.second.word == word) {
			return &i.second;
		}
	}

	return NULL;
}

Void Lex::PrintLine(Uint32 offset) {

	puts("");
	while (offset--) {
		printf("  ");
	}
}

LAME_END2
