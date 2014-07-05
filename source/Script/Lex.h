#ifndef __LAME_SCRIPT__PARSER_LEX__
#define __LAME_SCRIPT__PARSER_LEX__

#include "Define.h"

LAME_BEGIN2(Script)

using namespace ::LAME_NAMESPACE::Core;

typedef enum {
	kScriptLexDefault = -1,
	// variable/constants
	kScriptLexInt,
	kScriptLexFloat,
	kScriptLexString,
	// brackets
	kScriptLexBracketL,
	kScriptLexBracketR,
	kScriptLexParentheseL,
	kScriptLexParentheseR,
	kScriptLexArray,
	// selectors
	kScriptLexDirected,
	kScriptLexMediated,
	// single-argument
	kScriptLexIncrement,
	kScriptLexDecrement,
	kScriptLexSizeof,
	kScriptLexReturn,
	kScriptLexBitNot,
	kScriptLexNot,
	kScriptLexUnaryMinus,
	kScriptLexUnaryPlus,
	kScriptLexNew,
	// constructions
	kScriptLexIf,
	kScriptLexElse,
	kScriptLexWhile,
	kScriptLexDo,
	kScriptLexFor,
	kScriptLexTry,
	kScriptLexCatch,
	kScriptLexFinally,
	// math
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
	// bool
	kScriptLexBellow,
	kScriptLexAbove,
	kScriptLexBellowEqual,
	kScriptLexAboveEqual,
	kScriptLexEqual,
	kScriptLexNotEqual,
	kScriptLexAnd,
	kScriptLexOr,
	// special
	kScriptLexComma,
	kScriptLexCommentL,
	kScriptLexCommentR,
	kScriptLexLineComment,
	kScriptLexQuote,
	kScriptLexApostrophe,
	kScriptLexSemicolon,
	kScriptLexBraceL,
	kScriptLexBraceR,
	// keywords
	kScriptLexClass,
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
	// amount
	kScriptLexAmount
} LexID;

typedef enum {
	kScriptLexFlagDefault = 0x0000,
	kScriptLexFlagLeft = 0x0001,
	kScriptLexFlagRight = 0x0002,
	kScriptLexFlagConst = 0x0004,
	kScriptLexFlagMath = 0x0008,
	kScriptLexFlagBool = 0x0010,
	kScriptLexFlagCondition = 0x0020,
	kScriptLexFlagLanguage = 0x0040,
	kScriptLexFlagModificator = 0x0080,
	kScriptLexFlagUnknown = 0x0100,
	kScriptLexFlagWoParenthese = 0x0200
} LexFlagID;

class LAME_API Lex {
public:
	const Buffer word;
	const Uint32 priority;
	const LexID id;
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
	static Void PrintLine(Uint32 offset);
private:
	static Map<LexID, Lex> lexMap;
};

LAME_END2

#endif // ~__LAME_SCRIPT__PARSER_LEX__