#ifndef __LAME_SCRIPT__
#define __LAME_SCRIPT__

#include "Define.h"

LAME_BEGIN

typedef class ScriptPriority ScriptPriority, *ScriptPriorityPtr;
typedef class ScriptLanguage ScriptLanguage, *ScriptLanguagePtr;
typedef class ScriptType ScriptType, *ScriptTypePtr;
typedef class ScriptVariable ScriptVariable, *ScriptVariablePtr;
typedef class KeyWord KeyWord, *KeyWordPtr;
typedef class ScriptParser ScriptParser, *ScriptParserPtr;

typedef Sint32 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Bool ScriptNativeBool;
typedef Buffer ScriptNativeString;

typedef enum {
	kPriorityDefault = -1,
	kPriorityBracketL,
	kPriorityBracketR,
	kPriorityParenthesesL,
	kPriorityParenthesesR,
	kPriorityDot,
    kPriorityArrow,
	kPriorityDeclare,
	kPriorityIncrement,
	kPriorityDecrement,
	kPrioritySizeof,
	kPriorityBitNot,
	kPriorityNot,
	/*kPriorityUnaryMinus,
	kPriorityUnaryPlus,
	kPriorityOffset,
	kPriorityClaim,*/
	kPriorityType,
	kPriorityMul,
	kPriorityDiv,
	kPriorityMod,
	kPriorityAdd,
	kPrioritySub,
	kPriorityShiftL,
	kPriorityShiftR,
	kPriorityBellowEqual,
	kPriorityAboveEqual,
	kPriorityBellow,
	kPriorityAbove,
	kPriorityEqual,
	kPriorityNotEqual,
	kPriorityBitAnd,
	kPriorityBitXor,
	kPriorityBitOr,
	kPriorityAnd,
	kPriorityOr,
	kPrioritySet,
	kPriorityAddSet,
	kPrioritySubSet,
	kPriorityMulSet,
	kPriorityDivSet,
	kPriorityModSet,
	kPriorityShiftSetL,
	kPriorityShiftSetR,
	kPriorityBitAndSet,
	kPriorityBitXorSet,
	kPriorityBitOrSet,
	kPriorityComa,
	kPriorityVariable,
	kPriorityLanguage,
	kPriorityAmount
} ScriptPriorityEnum;

static StringC languagePriority[kPriorityAmount] = {
	"[", // postfix
	"]",
	"(",
	")",
	".",
	"->",
	"${type}",
	"++",
	"--",
	"sizeof", // unary
	"~",
    "!",
	/*"-",
	"+",
	"&",
	"*",*/
	"${type}",
	"*", // binary
	"/",
	"%",
	"+",
	"-",
	"<<",
	">>",
	"<=",
	">=",
	"<",
	">",
	"==",
	"!=",
	"&",
	"^",
	"|",
	"&&",
	"||",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%=",
	"<<=",
	">>=",
	"&=",
	"^=",
	"|=",
	",",
	"${language}",
	"${variable}",
};

typedef enum {
	kScriptLanguageDefault = -1,
	kScriptLanguageIf,
	kScriptLanguageElse,
	kScriptLanguageWhile,
	kScriptLanguageAmount
} ScriptLanguages;

namespace internal {
	static StringC __languageStrings[kScriptLanguageAmount] = {
		"if",
		"else",
		"while"
	};
}

typedef enum {
	kScriptTypeDefault = -1,
	kScriptTypeInt,
	kScriptTypeFloat,
	kScriptTypeBool,
	kScriptTypeString,
	kScriptTypeVoid,
	kScriptTypeAmount
} ScriptTypes;

namespace internal {
	static StringC __typeStrings[kScriptTypeAmount] = {
		"Int",
		"Float",
		"Bool",
		"String",
		"Void"
	};
}

typedef enum {
	kKeyWordDefault = -1,
	kKeyWordPriority,
	kKeyWordSeparator,
	kKeyWordType,
	kKeyWordLanguage,
	kKeyWordInt,
	kKeyWordFloat,
	kKeyWordString,
	kKeyWordAmount
} KeyWords;

class LAME_API ScriptPriority {
public:
	ScriptPriority& Parse(StringC word);
public:
	inline ScriptPriority& Parse(BufferRefC word) {
		return this->Parse(word.data());
	}
	inline StringC String() {
		return languagePriority[this->priority_];
	}
public:
	inline Uint32 Priority() {
		if ((this->priority_ >= kPriorityMul && this->priority_ <= kPriorityBitOrSet) ||
			this->priority_ == kPriorityIncrement ||
			this->priority_ == kPriorityDecrement
		) {
			return this->priority_;
		} else {
			return kPriorityAmount - (Sint32)this->priority_;
		}
	}
	inline Uint32 Arguments() {
		if (this->IsUnary() ||
			this->IsPostfix()) {
			return 1;
		} else if (this->IsBinary()) {
			return 2;
		} else {
			return 0;
		}
	}
public:
	inline Bool IsLeftAssociated() {
        return
            (this->priority_ >= kPrioritySet &&
             this->priority_ <= kPriorityBitOrSet) ||
             this->priority_ == kPriorityIncrement ||
             this->priority_ == kPriorityDecrement;
	}
	inline Bool IsRightAssociated() {
        return !this->IsLeftAssociated();
	}
public:
	inline operator ScriptPriorityEnum () const { return this->priority_; }
	inline ScriptPriority operator = (ScriptPriorityEnum p) { this->priority_ = p; return *this; }
public:
	inline Bool IsPostfix() {
		return this->priority_ >= kPriorityBracketL && this->priority_ <= kPriorityDecrement;
	}
	inline Bool IsUnary() {
		return this->priority_ >= kPrioritySizeof && this->priority_ <= kPriorityType;
	}
	inline Bool IsBinary() {
		return this->priority_ >= kPriorityMul && this->priority_ <= kPriorityComa;
	}
	inline Bool IsType() {
		return this->priority_ == kPriorityType;
	}
private:
	ScriptPriorityEnum priority_;
};

class ScriptLanguage {
public:
	StringC String();
	ScriptLanguage& Parse(StringC word);
public:
	inline operator ScriptLanguages () const { return this->type_; }
	inline ScriptLanguage operator = (const ScriptLanguages& type) { this->type_ = type; return *this; }
public:
	ScriptLanguage() {
		this->type_ = kScriptLanguageDefault;
	}
	ScriptLanguage(const ScriptLanguages& type) {
		this->type_ = type;
	}
private:
	ScriptLanguages type_;
};

class ScriptType {
public:
	StringC String();
	ScriptType& Parse(StringC word);
public:
	inline operator ScriptTypes () const { return this->type_; }
	inline ScriptType operator = (const ScriptTypes& type) { this->type_ = type; return *this; }
public:
	ScriptType() {
		this->type_ = kScriptTypeDefault;
	}
private:
	ScriptTypes type_;
};

class LAME_API ScriptVariable {
public:
	inline Void operator = (const ScriptNativeInt& value) {
		this->intValue = value; this->type = kScriptTypeInt;
	}
	inline Void operator = (const ScriptNativeFloat& value) {
		this->floatValue = value; this->type = kScriptTypeFloat;
	}
	inline Void operator = (const ScriptNativeBool& value) {
		this->boolValue = value; this->type = kScriptTypeBool;
	}
	inline Void operator = (const ScriptNativeString& value) {
		this->stringValue = value; this->type = kScriptTypeString;
	}
public:
	Void Set(const ScriptVariable& value);
	Void Add(const ScriptVariable& value);
	Void Sub(const ScriptVariable& value);
	Void Mul(const ScriptVariable& value);
	Void Div(const ScriptVariable& value);
	Void Mod(const ScriptVariable& value);
public:
	Void BitAnd(const ScriptVariable& value);
	Void BitOr(const ScriptVariable& value);
	Void BitXor(const ScriptVariable& value);
	Void BitShiftR(const ScriptVariable& value);
	Void BitShiftL(const ScriptVariable& value);
public:
	Void And(const ScriptVariable& value);
	Void Or(const ScriptVariable& value);
	Void Above(const ScriptVariable& value);
	Void Bellow(const ScriptVariable& value);
	Void AboveEqual(const ScriptVariable& value);
	Void BellowEqual(const ScriptVariable& value);
	Void Equal(const ScriptVariable& value);
	Void NotEqual(const ScriptVariable& value);
public:
	Void BitNot();
	Void Not();
public:
	Void Inc();
	Void Dec();
public:
    Void ToBool();
public:
	static Bool Check(
        const ScriptVariable& left,
        const ScriptVariable& right);
public:
	static Void Convert(
              ScriptVariable& left,
        const ScriptVariable& right);
public:
	Void Reset() {
		this->type = kScriptTypeDefault;
		this->priority = kPriorityDefault;
		this->language = kScriptLanguageDefault;
		this->line = 0;
		this->jump = 0;
		this->registered = 0;
		this->boolValue = 0;
		this->floatValue = 0;
		this->intValue = 0;
		this->stringValue.clear();
		this->word.clear();
	}
public:
	ScriptNativeBool boolValue;
	ScriptNativeFloat floatValue;
	ScriptNativeInt intValue;
	ScriptNativeString stringValue;
public:
	ScriptVariable() {
		this->Reset();
	}
public:
	Bool registered;
	Buffer word;
	ScriptType type;
	Uint32 line;
	Sint32 jump;
	ScriptPriority priority;
	ScriptLanguage language;
};

class LAME_API KeyWord : public ScriptVariable {
public:
	KeyWord& Parse(Uint32 line, StringC word);
public:
	inline KeyWord& Parse(Uint32 line, const Buffer& word) {
		return this->Parse(line, word.data());
	}
public:
	KeyWords key;
};

class LAME_API ScriptParser {
	friend class ScriptPerformer;
public:
    ScriptParser& Load(StringC filename);
	ScriptParser& Parse(StringC script);
	ScriptParser& Trace(Bool flag = 0);
	ScriptParser& Sort(Void);
public:
	inline Void Parse(const Buffer& script) {
		this->Parse(script.data());
	}
private:
	List <KeyWord> keyQueue;
	Vector <KeyWordPtr> yardQueue;
};

class LAME_API ScriptPerformer {
public:
	ScriptPerformer& EvaluateConstants(ScriptParser& parser);
	ScriptPerformer& FixBraces(ScriptParser& parser);
	ScriptPerformer& ComputeJumps(ScriptParser& parser);
	ScriptPerformer& EvaluateDouble(KeyWordPtr& left, KeyWordPtr right, KeyWordPtr key);
	ScriptPerformer& EvaluateSingle(KeyWordPtr& left, KeyWordPtr key);
	ScriptPerformer& EvaluateScript(ScriptParser& parser);
    ScriptPerformer& RegisterVariables(ScriptParser& parser);
    ScriptPerformer& EvaluateConstruction(List <KeyWordPtr>* stack, KeyWordPtr kw);
public:
	ScriptPerformer& Trace(Void);
public:
	ScriptPerformer() {
		//this->temporary.Reset();
	}
private:
	Void RegisterVariable(KeyWordPtr kw);
	ScriptVariablePtr FindVariable(StringC name);
private:
	List<KeyWord> tempList;
	Map<Buffer, ScriptVariablePtr> varMap;
    //KeyWord temporary;
    Bool isJump = 0;
    Bool isJumpPrev = 0;
};

LAME_END

#endif // ~__LAME_SCRIPT__