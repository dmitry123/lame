#include "Lex.h"

LAME_BEGIN2(Script)

#define P(_priority, _reverse) _reverse ? Uint32(20 - _priority) : Uint32(_priority)

Map<LexID, Lex> Lex::lexMap = {

		/* Default */

		{ kScriptLexUnknown, {
			"#", P(0, 1), kScriptLexUnknown, 0,
				kScriptLexFlagRight | kScriptLexFlagUnknown | kScriptLexFlagExpression }
		},

		/* Constants */

		{ kScriptLexInt, {
			"#", P(0, 1), kScriptLexInt, 0,
				kScriptLexFlagRight | kScriptLexFlagConst | kScriptLexFlagExpression }
		},
		{ kScriptLexFloat, {
			"#", P(0, 1), kScriptLexFloat, 0,
				kScriptLexFlagRight | kScriptLexFlagConst | kScriptLexFlagExpression }
		},
		{ kScriptLexString, {
			"#", P(0, 1), kScriptLexString, 0,
				kScriptLexFlagRight | kScriptLexFlagConst | kScriptLexFlagExpression }
		},
		{ kScriptLexCharacter, {
			"#", P(0, 1), kScriptLexCharacter, 0,
				kScriptLexFlagRight | kScriptLexFlagConst | kScriptLexFlagExpression }
		},

		/* Brackets */

		{ kScriptLexBracketL,
			{ "[", P(2, 0), kScriptLexBracketL, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},
		{ kScriptLexBracketR,
			{ "]", P(2, 0), kScriptLexBracketR, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},
		{ kScriptLexParenthesisL,
			{ "(", P(2, 0), kScriptLexParenthesisL, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},
		{ kScriptLexParenthesisR,
			{ ")", P(2, 0), kScriptLexParenthesisR, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},

		/* Specials */

		{ kScriptLexCast, {
			"#", P(3, 1), kScriptLexCast, 1,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexArray,
			{ "[]", P(0, 0), kScriptLexArray, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},
		{ kScriptLexArgumentList,
			{ "...", P(0, 0), kScriptLexArgumentList, 0,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},

		/* Selection */

		{ kScriptLexDirected,
			{ ".", P(2, 1), kScriptLexDirected, 2,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},

		/* Unary Operators */

		{ kScriptLexPostfixIncrement,
			{ "++", P(2, 1), kScriptLexPostfixIncrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexPostfixDecrement,
			{ "--", P(2, 1), kScriptLexPostfixDecrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexPrefixIncrement,
			{ "++", P(3, 1), kScriptLexPrefixIncrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexPrefixDecrement,
			{ "--", P(3, 1), kScriptLexPrefixDecrement, 1,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexInstanceof,
			{ "instanceof", P(3, 0), kScriptLexInstanceof, 1,
				kScriptLexFlagRight | kScriptLexFlagExpression }
		},
		{ kScriptLexReturn,
			{ "return", P(0, 0), kScriptLexReturn, 1,
				kScriptLexFlagLanguage | kScriptLexFlagRight }
		},
		{ kScriptLexAssert,
			{ "assert", P(0, 0), kScriptLexAssert, 1,
				kScriptLexFlagLanguage | kScriptLexFlagRight }
		},
		{ kScriptLexBitNot,
			{ "~", P(3, 1), kScriptLexBitNot, 1,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexNot,
			{ "!", P(3, 1), kScriptLexNot, 1,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexUnaryMinus,
			{ "#", P(3, 1), kScriptLexUnaryMinus, 1,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexUnaryPlus,
			{ "#", P(3, 1), kScriptLexUnaryPlus, 1,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }	
		},
		{ kScriptLexNew,
			{ "new", P(3, 1), kScriptLexNew, 1,
				kScriptLexFlagRight | kScriptLexFlagLanguage | kScriptLexFlagExpression }
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
			{ "do", P(0, 1), kScriptLexDo, 1,
				kScriptLexFlagCondition | kScriptLexFlagLanguage | kScriptLexFlagWoParentheses }
		},
		{ kScriptLexFor,
			{ "for", P(0, 1), kScriptLexFor, 1,
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
		{ kScriptLexThrow,
			{ "throw", P(0, 0), kScriptLexThrow, 1,
				kScriptLexFlagLanguage | kScriptLexFlagExpression }
		},
		{ kScriptLexThrows,
			{ "throws", P(0, 0), kScriptLexThrows, 0,
				kScriptLexFlagLanguage| kScriptLexFlagExpression }
		},
		{ kScriptLexSwitch,
			{ "switch", P(0, 0), kScriptLexSwitch, 0,
				kScriptLexFlagLanguage | kScriptLexFlagCondition }
		},

		/* Binary Operators */

		{ kScriptLexMul,
			{ "*", P(5, 1), kScriptLexMul, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexDiv,
			{ "/", P(5, 1), kScriptLexDiv, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexMod,
			{ "%", P(5, 1), kScriptLexMod, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexAdd,
			{ "+", P(6, 1), kScriptLexAdd, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexSub,
			{ "-", P(6, 1), kScriptLexSub, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftL,
			{ "<<", P(7, 1), kScriptLexBitShiftL, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftR,
			{ ">>", P(7, 1), kScriptLexBitShiftR, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftU,
			{ ">>>", P(7, 1), kScriptLexBitShiftU, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitAnd,
			{ "&", P(10, 1), kScriptLexBitAnd, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitXor,
			{ "^", P(11, 1), kScriptLexBitXor, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitOr,
			{ "|", P(12, 1), kScriptLexBitOr, 2,
				kScriptLexFlagRight | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexSet,
			{ "=", P(16, 1), kScriptLexSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexAddSet,
			{ "+=", P(16, 1), kScriptLexAddSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexSubSet,
			{ "-=", P(16, 1), kScriptLexSubSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexMulSet,
			{ "*=", P(16, 1), kScriptLexMulSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexDivSet,
			{ "/=", P(16, 1), kScriptLexDivSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexModSet,
			{ "%=", P(16, 1), kScriptLexModSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftSetL,
			{ "<<=", P(16, 1), kScriptLexBitShiftSetL, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftSetR,
			{ ">>=", P(16, 1), kScriptLexBitShiftSetR, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitShiftSetU,
			{ ">>>=", P(16, 1), kScriptLexBitShiftSetU, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitAndSet,
			{ "&=", P(16, 1), kScriptLexBitAndSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitXorSet,
			{ "^=", P(16, 1), kScriptLexBitXorSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBitOrSet,
			{ "|=", P(16, 1), kScriptLexBitOrSet, 2,
				kScriptLexFlagLeft | kScriptLexFlagMath | kScriptLexFlagLogic | kScriptLexFlagExpression }
		},
		{ kScriptLexBelow,
			{ "<", P(8, 1), kScriptLexBelow, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexAbove,
			{ ">", P(8, 1), kScriptLexAbove, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexBelowEqual,
			{ "<=", P(8, 1), kScriptLexBelowEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexAboveEqual,
			{ ">=", P(8, 1), kScriptLexAboveEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexEqual,
			{ "==", P(9, 1), kScriptLexEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexNotEqual,
			{ "!=", P(9, 1), kScriptLexNotEqual, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexAnd,
			{ "&&", P(13, 1), kScriptLexAnd, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexOr,
			{ "||", P(14, 1), kScriptLexOr, 2,
				kScriptLexFlagRight | kScriptLexFlagBool | kScriptLexFlagExpression }
		},
		{ kScriptLexTernary,
			{ "?", P(15, 1), kScriptLexTernary, 1,
				kScriptLexFlagDefault | kScriptLexFlagMath | kScriptLexFlagExpression }
		},

		/* Others */

		{ kScriptLexComma,
			{ ",", P(18, 1), kScriptLexComma, 2,
				kScriptLexFlagRight | kScriptLexFlagExpression }
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
				kScriptLexFlagDefault | kScriptLexFlagExpression }
		},
		{ kScriptLexApostrophe,
			{ "\'", P(0, 0), kScriptLexApostrophe, 0,
				kScriptLexFlagDefault | kScriptLexFlagExpression }
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
		{ kScriptLexAnnotation,
			{ "@interface", P(0, 0), kScriptLexAnnotation, 0,
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
		{ kScriptLexSynchronized,
			{ "synchronized", P(0, 0), kScriptLexSynchronized, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator | kScriptLexFlagCondition }
		},
		{ kScriptLexDefault,
			{ "default", P(0, 0), kScriptLexDefault, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexCase,
			{ "case", P(0, 0), kScriptLexCase, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexPackage,
			{ "package", P(0, 0), kScriptLexPackage, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexImport,
			{ "import", P(0, 0), kScriptLexImport, 0,
				kScriptLexFlagLanguage }
		},
		{ kScriptLexGoto,
			{ "#", P(0, 0), kScriptLexGoto, 0,
				kScriptLexFlagUnknown }
		},
		{ kScriptLexTransient,
			{ "transient", P(0, 0), kScriptLexTransient, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		},
		{ kScriptLexVolatile,
			{ "volatile", P(0, 0), kScriptLexVolatile, 0,
				kScriptLexFlagLanguage | kScriptLexFlagModificator }
		}
};

LexPtrC Lex::Find(LexID id) {

	Map<LexID, Lex>::iterator i;

	static LexPtr unknownLex = NULL;

	if (!unknownLex) {
		unknownLex = &lexMap.find(kScriptLexUnknown)->second;
	}

	if (id == kScriptLexUnknown) {
		return unknownLex;
	}

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

List<LexPtr> Lex::Match(BufferRefC entry) {

	List<LexPtr> result;

	for (auto& i : lexMap) {
		if (strstr(entry.data(), i.second.word.data()) == entry.data()) {
            result.push_back(&i.second);
		}
	}

	return result;
}

Void Lex::PrintLine(Uint32 offset) {

	puts("");
	while (offset--) {
		printf("  ");
	}
}

LAME_END2
