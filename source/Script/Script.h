#ifndef __LAME_SCRIPT__
#define __LAME_SCRIPT__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN

typedef enum {
	kScriptObjectDefault = -1,
	kScriptObjectVariable,
	kScriptObjectInt,
	kScriptObjectFloat,
	kScriptObjectString,
	kScriptObjectBool,
	kScriptObjectBracketL,
	kScriptObjectBracketR,
	kScriptObjectParentheseL,
	kScriptObjectParentheseR,
	kScriptObjectDirect,
	kScriptObjectMediated,
	kScriptObjectIncrement,
	kScriptObjectDecrement,
	kScriptObjectSizeof,
	kScriptObjectBitNot,
	kScriptObjectNot,
	kScriptObjectUnaryMinus,
	kScriptObjectUnaryPlus,
	kScriptObjectType,
	kScriptObjectIf,
	kScriptObjectElse,
	kScriptObjectWhile,
	kScriptObjectDo,
	kScriptObjectFor,
	kScriptObjectFunction,
	kScriptObjectMul,
	kScriptObjectDiv,
	kScriptObjectMod,
	kScriptObjectAdd,
	kScriptObjectSub,
	kScriptObjectBitShiftL,
	kScriptObjectBitShiftR,
	kScriptObjectBellow,
	kScriptObjectAbove,
	kScriptObjectBellowEqual,
	kScriptObjectAboveEqual,
	kScriptObjectEqual,
	kScriptObjectNotEqual,
	kScriptObjectBitAnd,
	kScriptObjectBitXor,
	kScriptObjectBitOr,
	kScriptObjectAnd,
	kScriptObjectOr,
	kScriptObjectSet,
	kScriptObjectAddSet,
	kScriptObjectSubSet,
	kScriptObjectMulSet,
	kScriptObjectDivSet,
	kScriptObjectModSet,
	kScriptObjectBitShiftSetL,
	kScriptObjectBitShiftSetR,
	kScriptObjectBitAndSet,
	kScriptObjectBitXorSet,
	kScriptObjectBitOrSet,
	kScriptObjectComa,
	kScriptObjectCommentL,
	kScriptObjectCommentR,
	kScriptObjectLineComment,
	kScriptObjectQuote,
	kScriptObjectApostrophe,
	kScriptObjectSemicolon,
	kScriptObjectBraceL,
	kScriptObjectBraceR,
	kScriptObjectClass,
	kScriptObjectImplements,
	kScriptObjectExtends,
	kScriptObjectPublic,
	kScriptObjectPrivate,
	kScriptObjectProtected,
	kScriptObjectFinal,
	kScriptObjectOverride,
	kScriptObjectAbstract,
	kScriptObjectStatic,
	kScriptObjectBreak,
	kScriptObjectContinue,
	kScriptObjectTry,
	kScriptObjectCatch,
	kScriptObjectThrow,
	kScriptObjectAmount
} EScriptObject;

typedef enum {
	kScriptAssociativityDefault = -1,
	kScriptAssociativityLeft,
	kScriptAssociativityRight,
	kScriptAssociativityAmount
} EScriptAssociativity;

typedef enum {
	kScriptTypeDefault,
	kScriptTypeVoid,
	kScriptTypeInt,
	kScriptTypeFloat,
	kScriptTypeBool,
	kScriptTypeString,
	kScriptTypeFunction,
	kScriptTypeClass,
	kScriptTypeAmount
} EScriptType;

typedef enum {
	kScriptModificatorDefault = 0x0000,
	kScriptModificatorAbstract = 0x0001,
	kScriptModificatorFinal = 0x0002,
	kScriptModificatorOverride = 0x0004,
	kScriptModificatorPublic = 0x0008,
	kScriptModificatorPrivate = 0x0010,
	kScriptModificatorProtected = 0x0020,
	kScriptModificatorStatic = 0x0040
} EScriptModificator;

typedef enum {
	kScriptNodeDefault = -1,
	kScriptNodeCondition,
	kScriptNodeClass,
	kScriptNodeFunction,
	kScriptNodeExpression,
	kScriptNodeAmount
} EScriptNode;

typedef class ScriptType ScriptType, *ScriptTypePtr;
typedef class ScriptObject ScriptObject, *ScriptObjectPtr;
typedef class ScriptParser ScriptParser, *ScriptParserPtr;
typedef class ScriptVariable ScriptVariable, *ScriptVariablePtr;
typedef class ScriptNode ScriptNode, *ScriptNodePtr;
typedef class ScriptPerformer ScriptPerformer, *ScriptPerformerPtr;
typedef class ScriptBuilder ScriptBuilder, *ScriptBuilderPtr;
typedef class ScriptVirtualMachine ScriptVirtualMachine, *ScriptVirtualMachinePtr;

class LAME_API ScriptType {
public:
	inline operator EScriptType () const { return this->type; }
	inline Void operator = (EScriptType type) { this->type = type; }
public:
	Void Parse(StringC* word);
	StringC GetString(Void) const;
	Void Reset(Void);
public:
	Buffer name;
	EScriptType type;
};

class LAME_API ScriptObject {
public:
	inline Bool IsLeftAssociated() const { return this->associativity == kScriptAssociativityLeft; }
	inline Bool IsRightAssociated() const { return this->associativity == kScriptAssociativityRight; }
	inline Bool IsString() const { return this->type == kScriptTypeString; };
	inline Bool IsVoid() const { return this->type == kScriptTypeVoid; }
	inline Bool IsInt() const { return this->type == kScriptTypeInt; }
	inline Bool IsFloat() const { return this->type == kScriptTypeFloat; }
	inline Bool IsFunction() const { return this->type == kScriptTypeFunction; }
	inline Bool IsClass() const { return this->type == kScriptTypeClass; }
	inline Bool IsCondition() const { return this->object >= kScriptObjectIf && this->object <= kScriptObjectFor; }
	inline Bool IsModificator() const { return this->object >= kScriptObjectPublic && this->object <= kScriptObjectStatic; }
public:
	Void Parse(StringC* word);
	StringC GetString(Void) const;
	Void Reset(Void);
public:
	Buffer word;
	Uint32 priority;
	EScriptObject object;
	Uint32 arguments;
	EScriptAssociativity associativity;
	Uint32 line;
	ScriptType type;
public:
	static Bool IsDigit(Sint8 symbol);
	static Bool IsLetter(Sint8 symbol);
	static Bool IsIntValue(StringC string);
	static Bool IsHexValue(StringC string);
	static Bool IsStringValue(StringC string);
	static Bool IsIntValue(const Buffer& string);
	static Bool IsFloatValue(StringC string);
	static Bool IsFloatValue(const Buffer& string);
public:
	static ScriptNativeInt ParseIntValue(StringC string);
	static ScriptNativeFloat ParseFloatValue(StringC string);
	static ScriptNativeString ParseStringValue(StringC string);
public:
	inline static ScriptNativeInt ParseIntValue(const Buffer& string) {
		return ParseIntValue(string.data());
	}
	inline static ScriptNativeFloat ParseFloatValue(const Buffer& string) {
		return ParseFloatValue(string.data());
	}
	inline static ScriptNativeString ParseStringValue(const Buffer& string) {
		return ParseStringValue(string.data());
	}
public:
	static ScriptObjectPtr FindScriptObjectByFlag(EScriptObject object);
};

class LAME_API ScriptParser {
public:
	ScriptParser& Parse(StringC script);
	ScriptParser& Load(StringC filename);
public:
	Vector<ScriptObjectPtr> objectList;
};

class LAME_API ScriptVariable {
public:
	ScriptVariable(ScriptObjectPtr object) : object(object) {
		this->Reset();
	}
public:
	ScriptNativeBool boolValue;
	ScriptNativeInt intValue;
	ScriptNativeFloat floatValue;
	ScriptNativeString stringValue;
public:
	ScriptVariablePtr next;
	ScriptObjectPtr object;
	Uint32 modificators;
public:
	static Void ScriptVariable::Convert(
		      ScriptVariable& left,
		const ScriptVariable& right);
public:
	Void Reset(Void);
};

class LAME_API ScriptNode {
public:
	Void Order(ScriptPerformerPtr performer);
	Void Evaluate(ScriptPerformerPtr performer);
public:
	ScriptObjectPtr object = 0;
	ScriptNodePtr parent = 0;
	ScriptNodePtr next = 0;
	Bool result = 0;
	ScriptVariablePtr var = 0;
	EScriptNode type = kScriptNodeExpression;
public:
	Vector<ScriptNodePtr> condition;
	Vector<ScriptNodePtr> block;
private:
	Void Order(
		ScriptPerformerPtr performer,
		Vector<ScriptNodePtr>* list);
};

class LAME_API ScriptPerformer {
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
	Bool RegisterType(ScriptNodePtr node);
	Bool RegisterVar(ScriptNodePtr node);
	ScriptNodePtr FindType(StringC name);
	ScriptNodePtr FindVar(StringC name);
public:
	Void Evaluate(
		Vector<ScriptNodePtr>* list,
		Vector<ScriptNodePtr>* result);
	Void Trace(Void);
private:
	Void EvaluateSingleExpression(
		ScriptNodePtr left,
		ScriptNodePtr token);
	Void EvaluateDoubleExpression(
		ScriptNodePtr left,
		ScriptNodePtr right,
		ScriptNodePtr token);
public:
	ScriptNodePtr root = 0;
public:
	Vector<ScriptNodePtr> typeList;
	Vector<ScriptNodePtr> varList;
public:
	List<ScriptVariable> tempList;
};

class LAME_API ScriptBuilder {
	typedef Vector<ScriptObjectPtr>::iterator Iterator;
public:
	ScriptBuilder& Analyze(
		ScriptParserPtr parser,
		ScriptPerformerPtr performer);
private:
	Void AnalizeConstruction(
		ScriptNodePtr node,
		Iterator& i);
	Void BuildConstruction(
		ScriptNodePtr node,
		Iterator& i);
	Void BuildCondition(
		ScriptNodePtr node,
		Iterator& i);
public:
	ScriptParserPtr parser = 0;
	ScriptPerformerPtr performer = 0;
};

class LAME_API ScriptVirtualMachine {
public:
	ScriptVirtualMachine& Execute(ScriptPerformerPtr performer);
};

LAME_END

#endif // ~__LAME_SCRIPT__