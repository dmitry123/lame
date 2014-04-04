#ifndef __LAME_SCRIPT__
#define __LAME_SCRIPT__

#include "Define.h"

LAME_BEGIN

typedef Sint32 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Bool ScriptNativeBool;
typedef Buffer ScriptNativeString;

typedef class ScriptType ScriptType, *ScriptTypePtr;
typedef class ScriptTypeManager ScriptTypeManager, *ScriptTypeManagerPtr;
typedef class ScriptObject ScriptObject, *ScriptObjectPtr;
typedef class ScriptVariable ScriptVariable, *ScriptVariablePtr;
typedef class ScriptVariableManager ScriptVariableManager, *ScriptVariableManagerPtr;
typedef class ScriptNode ScriptNode, *ScriptNodePtr;
typedef class ScriptSorter ScriptSorter, *ScriptSorterPtr;
typedef class ScriptPerformer ScriptPerformer, *ScriptPerformerPtr;
typedef class ScriptParser ScriptParser, *ScriptParserPtr;

typedef enum {
	kScriptDefault = -1,
	/* FIRST */
	kScriptVariable,
	kScriptInt,
	kScriptFloat,
	kScriptString,
	/* POSTFIX */
	kScriptBracketL,
	kScriptBracketR,
	kScriptParentheseL,
	kScriptParentheseR,
	kScriptDirectSelection,
	kScriptMediatedSelection,
	kScriptDeclare,
	kScriptIncrement,
	kScriptDecrement,
	/* UNARY */
	kScriptSizeof,
	kScriptBitNot,
	kScriptNot,
	kScriptUnaryMinus,
	kScriptUnaryPlus,
	kScriptOffset,
	kScriptClaim,
	kScriptType,
	kScriptIf,
	kScriptElse,
	kScriptWhile,
	kScriptDo,
	kScriptFor,
	/* BINARY */
	kScriptMul,
	kScriptDiv,
	kScriptMod,
	kScriptAdd,
	kScriptSub,
	kScriptBitShiftL,
	kScriptBitShiftR,
	kScriptBellow,
	kScriptAbove,
	kScriptBellowEqual,
	kScriptAboveEqual,
	kScriptEqual,
	kScriptNotEqual,
	kScriptBitAnd,
	kScriptBitXor,
	kScriptBitOr,
	kScriptAnd,
	kScriptOr,
	kScriptSet,
	kScriptAddSet,
	kScriptSubSet,
	kScriptMulSet,
	kScriptDivSet,
	kScriptModSet,
	kScriptBitShiftSetL,
	kScriptBitShiftSetR,
	kScriptBitAndSet,
	kScriptBitXorSet,
	kScriptBitOrSet,
	kScriptComa,
	/* OTHER */
	kScriptCommentL,
	kScriptCommentR,
	kScriptCommentLine,
	kScriptQuote,
	kScriptApostrophe,
	kScriptSemicolon,
	kScriptBraceL,
	kScriptBraceR,
	kScriptAmount
} ScriptFlag;

typedef enum {
	kScriptArgDefault = -1,
	kScriptArgPostfix,
	kScriptArgUnary,
	kScriptArgBinary
} ScriptArg;

typedef enum {
	kScriptAssociativityDefault = -1,
	kScriptAssociativityLeft,
	kScriptAssociativityRight
} ScriptAssociativity;

typedef enum {
	kScriptTypeDefault = -1,
	kScriptTypeVoid,
	kScriptTypeInt,
	kScriptTypeFloat,
	kScriptTypeBool,
	kScriptTypeString,
	kScriptTypeFunction,
	kScriptTypeObject,
	kScriptTypeAmount
} ScriptTypeEnum;

class LAME_API ScriptType {
public:
	Buffer word;
	ScriptTypeEnum type;
	ScriptVariablePtr var;
public:
	inline Void operator = (const ScriptTypeEnum& type) { this->type = type; }
	inline operator ScriptTypeEnum (Void) const { return this->type; };
public:
	ScriptType& Parse(StringC* word);
	StringC GetString(Void) const;
};

class LAME_API ScriptTypeManager {
	typedef Buffer* BufferPtr;
public:
	Bool Declare(const ScriptVariable& type);
	ScriptTypePtr Find(StringC name);
public:
	Map<Buffer, ScriptType> typeMap;
};

class LAME_API ScriptObject {
public:
	Buffer word;
	Uint32 priority;
	ScriptFlag flag;
	ScriptArg args;
	ScriptAssociativity associativity;
	Uint32 line;
    ScriptVariablePtr var;
public:
	ScriptObject& Parse(StringC* word);
	StringC GetString(Void) const;
public:
	inline Bool IsLeftAssociated() const { return this->associativity == kScriptAssociativityLeft; }
	inline Bool IsRightAssociated() const { return this->associativity == kScriptAssociativityRight; }
	inline Bool IsPostfix() const { return this->args == kScriptArgPostfix; }
	inline Bool IsUnary() const { return this->args == kScriptArgUnary; }
	inline Bool IsBinary() const { return this->args == kScriptArgBinary; }
    inline Bool IsCondition() const { return this->flag >= kScriptIf && this->flag <= kScriptFor; }
	inline Bool IsArgsBegin() const { return this->flag == kScriptParentheseL; }
	inline Bool IsArgsEnd() const { return this->flag == kScriptParentheseR; }
	inline Bool IsBlockBegin() const { return this->flag == kScriptBraceL; }
	inline Bool IsBlockEnd() const { return this->flag == kScriptBraceR; }
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
	static Void Convert(
		      ScriptVariable& left,
		const ScriptVariable& right);
public:
    ScriptVariable& Reset(Void);
public:
	ScriptNativeInt intValue;
	ScriptNativeFloat floatValue;
	ScriptNativeBool boolValue;
	ScriptNativeString stringValue;
public:
	ScriptType type;
	ScriptObjectPtr object = 0;
};

class LAME_API ScriptVariableManager {
public:
	Bool Declare(ScriptVariable variable);
	ScriptVariablePtr Find(StringC name);
public:
	Map<Buffer, ScriptVariable> varMap;
};

class LAME_API ScriptNode {
public:
    Void Order(Void);
    Void Evaluate(ScriptPerformerPtr performer);
public:
	ScriptObjectPtr object = 0;
	ScriptNodePtr parent = 0;
	Bool result = 0;
public:
    Vector<ScriptNodePtr> condition;
	Vector<ScriptNodePtr> block;
private:
	Void Order(Vector<ScriptNodePtr>* list);
};

class LAME_API ScriptPerformer {
	friend class ScriptParser;
public:
	Void Set(ScriptVariable& left, const ScriptVariable& right);
	Void Add(ScriptVariable& left, const ScriptVariable& right);
	Void Sub(ScriptVariable& left, const ScriptVariable& right);
	Void Mul(ScriptVariable& left, const ScriptVariable& right);
	Void Div(ScriptVariable& left, const ScriptVariable& right);
	Void Mod(ScriptVariable& left, const ScriptVariable& right);
	Void BitAnd(ScriptVariable& left, const ScriptVariable& right);
	Void BitOr(ScriptVariable& left, const ScriptVariable& right);
	Void BitXor(ScriptVariable& left, const ScriptVariable& right);
	Void BitShiftR(ScriptVariable& left, const ScriptVariable& right);
	Void BitShiftL(ScriptVariable& left, const ScriptVariable& right);
	Void And(ScriptVariable& left, const ScriptVariable& right);
	Void Or(ScriptVariable& left, const ScriptVariable& right);
	Void Above(ScriptVariable& left, const ScriptVariable& right);
	Void Bellow(ScriptVariable& left, const ScriptVariable& right);
	Void AboveEqual(ScriptVariable& left, const ScriptVariable& right);
	Void BellowEqual(ScriptVariable& left, const ScriptVariable& right);
	Void Equal(ScriptVariable& left, const ScriptVariable& right);
	Void NotEqual(ScriptVariable& left, const ScriptVariable& right);
	Void BitNot(ScriptVariable& left);
	Void Not(ScriptVariable& left);
	Void Inc(ScriptVariable& left);
	Void Dec(ScriptVariable& left);
	Void AsBool(ScriptVariable& left);
public:
	ScriptPerformer& Evaluate(Void);
public:
	Void Evaluate(
		Vector<ScriptNodePtr>* list,
		Vector<ScriptNodePtr>* result);
private:
	Void EvaluateSingleExpression(
		ScriptNodePtr left,
		ScriptNodePtr token);
	Void EvaluateDoubleExpression(
		ScriptNodePtr left,
		ScriptNodePtr right,
		ScriptNodePtr token);
private:
	Vector<ScriptNodePtr> nodeTree;
	ScriptVariableManager varManager;
	ScriptTypeManager typeManager;
};

class LAME_API ScriptParser {
public:
	ScriptParser& Parse(StringC script);
	ScriptParser& Build(ScriptPerformerPtr performer);
	ScriptParser& Load(StringC file);
private:
	Void RegisterVariable(Void);
	Void RegisterType(Void);
    Void RegisterConstants(Void);
private:
	List<ScriptObject> keyList;
    List<ScriptVariable> varList;
	List<ScriptNode> nodeList;
};

LAME_END

#endif // ~__LAME_SCRIPT__