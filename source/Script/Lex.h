#ifndef __LAME_SCRIPT__PARSER_LEX__
#define __LAME_SCRIPT__PARSER_LEX__

#include "Define.h"

LAME_BEGIN2(Script)

typedef enum {
	/* Default */
	kScriptLexDefault = -1,
	/* Constants */
	kScriptLexInt,
	kScriptLexFloat,
	kScriptLexString,
	kScriptLexCharacter,
	/* Brackets */
	kScriptLexBracketL,
	kScriptLexBracketR,
	kScriptLexParenthesisL,
	kScriptLexParenthesisR,
	/* Specials */
	kScriptLexCast,
	kScriptLexArray,
	kScriptLexArgumentList,
	/* Selection */
	kScriptLexDirected,
	/* Unary Operatorss */
	kScriptLexIncrement,
	kScriptLexDecrement,
	kScriptLexPrefixIncrement,
	kScriptLexPrefixDecrement,
	kScriptLexInstanceof,
	kScriptLexReturn,
	kScriptLexBitNot,
	kScriptLexNot,
	kScriptLexUnaryMinus,
	kScriptLexUnaryPlus,
	kScriptLexNew,
	/* Language Constructions */
	kScriptLexIf,
	kScriptLexElse,
	kScriptLexWhile,
	kScriptLexDo,
	kScriptLexFor,
	kScriptLexTry,
	kScriptLexCatch,
	kScriptLexFinally,
	/* Binary Operators */
	kScriptLexMul,
	kScriptLexDiv,
	kScriptLexMod,
	kScriptLexAdd,
	kScriptLexSub,
	kScriptLexBitShiftL,
	kScriptLexBitShiftR,
	kScriptLexBitAnd,
	kScriptLexBitXor,
	kScriptLexBitOr,
	kScriptLexSet,
	kScriptLexAddSet,
	kScriptLexSubSet,
	kScriptLexMulSet,
	kScriptLexDivSet,
	kScriptLexModSet,
	kScriptLexBitShiftSetL,
	kScriptLexBitShiftSetR,
	kScriptLexBitAndSet,
	kScriptLexBitXorSet,
	kScriptLexBitOrSet,
	kScriptLexBellow,
	kScriptLexAbove,
	kScriptLexBellowEqual,
	kScriptLexAboveEqual,
	kScriptLexEqual,
	kScriptLexNotEqual,
	kScriptLexAnd,
	kScriptLexOr,
	/* Others */
	kScriptLexComma,
	kScriptLexCommentL,
	kScriptLexCommentR,
	kScriptLexLineComment,
	kScriptLexQuote,
	kScriptLexApostrophe,
	kScriptLexSemicolon,
	kScriptLexColon,
	kScriptLexBraceL,
	kScriptLexBraceR,
	/* Language Keywords */
	kScriptLexClass,
	kScriptLexEnum,
	kScriptLexInterface,
	kScriptLexImplements,
	kScriptLexExtends,
	kScriptLexBreak,
	kScriptLexContinue,
	kScriptLexPublic,
	kScriptLexPrivate,
	kScriptLexProtected,
	kScriptLexStatic,
	kScriptLexNative,
	kScriptLexFinal,
	kScriptLexAbstract,
	kScriptLexOverride,
	kScriptLexDecprecated,
	/* Count Of Lexes */
	kScriptLexAmount
} LexID;

typedef enum {
	/* No-Flags */
	kScriptLexFlagDefault = 0x0000,
	/* Left-Associated Lex */
	kScriptLexFlagLeft = 0x0001,
	/* Right-Associated Lex */
	kScriptLexFlagRight = 0x0002,
	/* Lex Is Constant */
	kScriptLexFlagConst = 0x0004,
	/* Math Like Operator */
	kScriptLexFlagMath = 0x0008,
	/* Bool Like Operator */
	kScriptLexFlagBool = 0x0010,
	/* Language Construction */
	kScriptLexFlagCondition = 0x0020,
	/* Reserved Language Specific Keyword */
	kScriptLexFlagLanguage = 0x0040,
	/* Modificator */
	kScriptLexFlagModificator = 0x0080,
	/* Something Unknown */
	kScriptLexFlagUnknown = 0x0100,
	/* Language Construction Has't Parentheses */
	kScriptLexFlagWoParentheses = 0x0200,
    /* Logic Mac Operators */
    kScriptLexFlagLogic = 0x0400
} LexFlagID;

class LAME_API Lex {
public:
	const Buffer word;
	const Uint32 priority;
	const LexID  id;
	const Uint32 args;
	const Uint32 flags;
public:
	inline Bool IsUnknown() const {
		return (this->flags & kScriptLexFlagUnknown) != 0;
	}
	inline Bool IsLeft() const {
		return (this->flags & kScriptLexFlagLeft) != 0;
	}
	inline Bool IsRight() const {
		return (this->flags & kScriptLexFlagRight) != 0;
	}
	inline Bool IsConst() const {
		return (this->flags & kScriptLexFlagConst) != 0;
	}
	inline Bool IsMath() const {
		return (this->flags & kScriptLexFlagMath) != 0;
	}
	inline Bool IsBool() const {
		return (this->flags & kScriptLexFlagBool) != 0;
	}
	inline Bool IsCondition() const {
		return (this->flags & kScriptLexFlagCondition) != 0;
	}
	inline Bool IsLanguage() const {
		return (this->flags & kScriptLexFlagLanguage) != 0;
	}
	inline Bool IsModificator() const {
		return (this->flags & kScriptLexFlagModificator) != 0;
	}
    inline Bool IsLogic() const {
		return (this->flags & kScriptLexFlagLogic) != 0;
    }
public:
	inline Bool IsClass() const {
		return this->id == kScriptLexClass;
	}
	inline Bool IsInterface() const {
		return this->id == kScriptLexInterface;
	}
public:
	static LexPtrC Find(LexID id);
	static LexPtrC Find(BufferRefC word);
	static Void PrintLine(Uint32 tabOffset = 0);
private:
	static Map<LexID, Lex> lexMap;
};

LAME_END2

#endif // ~__LAME_SCRIPT__PARSER_LEX__