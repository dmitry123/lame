#ifndef __LAME_SCRIPT__PARSER__
#define __LAME_SCRIPT__PARSER__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN

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
	// selectors
	kScriptLexDirected,
	kScriptLexMediated,
	// single-argument
	kScriptLexIncrement,
	kScriptLexDecrement,
	kScriptLexSizeof,
	kScriptLexBitNot,
	kScriptLexNot,
	kScriptLexUnaryMinus,
	kScriptLexUnaryPlus,
	kScriptLexType,
	kScriptLexNew,
	// constructions
	kScriptLexIf,
	kScriptLexElse,
	kScriptLexWhile,
	kScriptLexDo,
	kScriptLexFor,
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
	kScriptLexComa,
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
	kScriptLexImplements,
	kScriptLexBreak,
	kScriptLexContinue,
	kScriptLexAmount
} ScriptLexID;

typedef enum {
	kScriptLexFlagDefault = 0x0000,
	kScriptLexFlagLeft = 0x0001,
	kScriptLexFlagRight = 0x0002,
	kScriptLexFlagConst = 0x0004,
	kScriptLexFlagMath = 0x0008,
	kScriptLexFlagBool = 0x0010,
	kScriptLexFlagCondition = 0x0020,
	kScriptLexFlagLanguage = 0x0040,
	kScriptLexFlagUnknown = 0x0080
} ScriptLexFlagID;

class LAME_API ScriptLex {
public:
	const Buffer word;
	const Uint32 priority;
	const ScriptLexID id;
	const Uint32 args;
	const Uint32 flags;
public:
	inline Bool IsUnknown() { return (this->flags & kScriptLexFlagUnknown) != 0; }
	inline Bool IsLeft() { return (this->flags & kScriptLexFlagLeft) != 0; }
	inline Bool IsRight() { return (this->flags & kScriptLexFlagRight) != 0; }
	inline Bool IsConst() { return (this->flags & kScriptLexFlagConst) != 0; }
	inline Bool IsMath() { return (this->flags & kScriptLexFlagMath) != 0; }
	inline Bool IsBool() { return (this->flags & kScriptLexFlagBool) != 0; }
	inline Bool IsCondition() { return (this->flags & kScriptLexFlagCondition) != 0; }
	inline Bool IsLanguage() { return (this->flags & kScriptLexFlagLanguage) != 0; }
public:
	Uint32 line;
};

class LAME_API ScriptLexNode {
public:
	const Buffer word;
	const Uint32 line;
	const ScriptLexPtrC lex;
public:
	ScriptLexNode(BufferRefC word, Uint32 line, ScriptLexPtrC lex) :
		word(word),
		line(line),
		lex(lex)
	{
		if (lex) {
			lex->line = line;
		}
	}
};

class LAME_API ScriptParser {
	friend class ScriptBuilder;
public:
	Void Load(StringC file);
	Void Parse(StringC script);
public:
	static ScriptLexPtrC Find(ScriptLexID id);
	static ScriptLexPtrC Find(StringC word);
	static ScriptLexPtrC Find(BufferRefC word);
public:
	inline Vector<ScriptLexNodePtr>* const GetLexList() const {
		return (Vector<ScriptLexNodePtr>* const)&this->lexList_;
	}
private:
	ScriptLexPtrC _ParseLex(StringC* word, Buffer* name);
private:
	Vector<ScriptLexNodePtr> lexList_;
};

LAME_END

#endif // ~__LAME_SCRIPT__PARSER__