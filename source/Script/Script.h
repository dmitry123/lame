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
	kScriptObjectThread,
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
	kScriptTypeVar,
	kScriptTypeFunction,
	kScriptTypeClass,
	kScriptTypeAmount
} EScriptType;

typedef enum {
	kScriptModificatorDefault = 0x0001,
	kScriptModificatorAbstract = 0x0002,
	kScriptModificatorFinal = 0x0004,
	kScriptModificatorOverride = 0x0008,
	kScriptModificatorPublic = 0x0010,
	kScriptModificatorPrivate = 0x0020,
	kScriptModificatorProtected = 0x0040,
	kScriptModificatorStatic = 0x0080,
    kScriptModificatorVar = 0x0100,
	kScriptModificatorFunction = 0x0200
} EScriptModificator;

typedef enum {
	kScriptNodeDefault = -1,
	kScriptNodeCondition,
	kScriptNodeClass,
	kScriptNodeField,
	kScriptNodeExpression,
	kScriptNodeThread,
	kScriptNodeDeclare,
	kScriptNodeAmount
} EScriptNode;

typedef class ScriptType ScriptType, *ScriptTypePtr;
typedef class ScriptObject ScriptObject, *ScriptObjectPtr;
typedef class ScriptParser ScriptParser, *ScriptParserPtr;
typedef class ScriptVariable ScriptVariable, *ScriptVariablePtr;
typedef class ScriptNode ScriptNode, *ScriptNodePtr;
typedef class ScriptTypeManager ScriptTypeManager, *ScriptTypeManagerPtr;
typedef class ScriptVarManager ScriptVarManager, *ScriptVarManagerPtr;
typedef class ScriptManager ScriptManager, *ScriptManagerPtr;
typedef class ScriptPerformer ScriptPerformer, *ScriptPerformerPtr;
typedef class ScriptThread ScriptThread, *ScriptThreadPtr;
typedef class ScriptThreadManager ScriptThreadManager, *ScriptThreadManagerPtr;
typedef class ScriptBuilder ScriptBuilder, *ScriptBuilderPtr;
typedef class ScriptVirtualMachine ScriptVirtualMachine, *ScriptVirtualMachinePtr;
typedef class ScriptController ScriptController, *ScriptControllerPtr;

class LAME_API ScriptType {
	friend class ScriptTypeManager;
public:
	inline operator EScriptType () const { return this->type; }
	inline Void operator = (EScriptType type) { this->type = type; }
public:
	Void Parse(StringC* word);
	StringC GetString(Void) const;
	Void Reset(Void);
private:
	static ScriptTypePtr GetList(Void);
public:
	Buffer name;
	EScriptType type;
};

class LAME_API ScriptObject {
public:
	inline Bool IsLeftAssociated() const { return this->associativity == kScriptAssociativityLeft; }
	inline Bool IsRightAssociated() const { return this->associativity == kScriptAssociativityRight; }
	inline Bool IsType() const { return this->object == kScriptObjectType; }
	inline Bool IsString() const { return this->type == kScriptTypeString; };
	inline Bool IsVoid() const { return this->type == kScriptTypeVoid; }
	inline Bool IsInt() const { return this->type == kScriptTypeInt; }
	inline Bool IsFloat() const { return this->type == kScriptTypeFloat; }
	inline Bool IsClass() const { return this->object == kScriptObjectClass; }
	inline Bool IsCondition() const { return this->object >= kScriptObjectIf && this->object <= kScriptObjectFor; }
	inline Bool IsVariable() const { return this->object == kScriptObjectVariable || this->object == kScriptObjectDefault; }
	inline Bool IsModificator() const { return this->object >= kScriptObjectPublic && this->object <= kScriptObjectStatic; }
	inline Bool IsThread() const { return this->object == kScriptObjectThread; }
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
public:
	~ScriptParser() {
		for (ScriptObjectPtr object : this->objectList) {
			delete object;
		}
		this->objectList.clear();
	}
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
	ScriptObjectPtr object;
	Uint32 modificators;
	Bool declared;
public:
    inline Bool IsAbstract() const { return (this->modificators & kScriptModificatorAbstract); }
	inline Bool IsFinal() const { return (this->modificators & kScriptModificatorFinal); }
	inline Bool IsOverride() const { return (this->modificators & kScriptModificatorOverride); }
	inline Bool IsPublic() const { return (this->modificators & kScriptModificatorPublic); }
	inline Bool IsPrivate() const { return (this->modificators & kScriptModificatorPrivate); }
	inline Bool IsProtected() const { return (this->modificators & kScriptModificatorProtected); }
	inline Bool IsStatic() const { return (this->modificators & kScriptModificatorStatic); }
	inline Bool IsVar() const { return (this->modificators & kScriptModificatorVar); }
	inline Bool IsFunction() const { return (this->modificators & kScriptModificatorFunction); }
	inline Bool IsDeclared() const { return this->declared; }
public:
	Void Convert(ScriptType& type);
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
	ScriptNodePtr prev = 0;
	Bool result = 0;
	ScriptVariablePtr var = 0;
	EScriptNode type = kScriptNodeDefault;
public:
	Vector<ScriptNodePtr> condition;
	Vector<ScriptNodePtr> block;
private:
	Void Order(
		ScriptPerformerPtr performer,
		Vector<ScriptNodePtr>* list);
};

class LAME_API ScriptTypeManager {
	friend class ScriptPerformer;
public:
	typedef Map<Buffer, ScriptTypePtr> TypeMap, *TypeMapPtr;
public:
	Bool Declare(ScriptTypePtr type);
	ScriptTypePtr Find(const Buffer& typeName);
public:
	Void Push(Void);
	Void Pop(Void);
public:
	inline Uint32 GetCurrentDepth() {
		return this->spaceMapQueue.size();
	}
public:
	ScriptTypeManager() {
		this->Push(); this->_DeclareLanguageTypes();
	}
private:
	Void _DeclareLanguageTypes(Void);
private:
	TypeMapPtr spaceMap = 0;
	List<TypeMap> spaceMapQueue;
};

class LAME_API ScriptVarManager {
	friend class ScriptPerformer;
public:
	typedef Map<Buffer, ScriptVariablePtr> VarMap, *TypeVarPtr;
public:
	Bool Declare(ScriptVariablePtr var);
	ScriptVariablePtr Find(const Buffer& varName);
public:
	Void Push(Void);
	Void Pop(Void);
public:
	inline Uint32 GetCurrentDepth() {
		return this->spaceMapQueue.size();
	}
public:
	ScriptVarManager() {
		this->Push();
	}
private:
	TypeVarPtr spaceMap = 0;
	List<VarMap> spaceMapQueue;
};

class LAME_API ScriptManager {
public:
	inline Bool DeclareVar(ScriptVariablePtr var) {
		return this->varManager.Declare(var);
	}
	inline Bool DeclareType(ScriptTypePtr type) {
		return this->typeManager.Declare(type);
	}
	inline ScriptVariablePtr FindVar(const Buffer& name) {
		return this->varManager.Find(name);
	}
	inline ScriptTypePtr FindType(const Buffer& name) {
		return this->typeManager.Find(name);
	}
public:
	inline Void Push() {
		this->varManager.Push();
		this->typeManager.Push();
	}
	inline Void Pop() {
		this->varManager.Pop();
		this->typeManager.Pop();
	}
public:
	inline ScriptVarManagerPtr GetVarManager() const {
		return (ScriptVarManagerPtr)&this->varManager;
	}
	inline ScriptTypeManagerPtr GetTypeManager() const {
		return (ScriptTypeManagerPtr)&this->typeManager;
	}
private:
	ScriptVarManager varManager;
	ScriptTypeManager typeManager;
};

class LAME_API ScriptPerformer {
	friend class ScriptBuilder;
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
	Void RegisterType(ScriptNodePtr node);
	Void RegisterVar(ScriptNodePtr node);
	ScriptTypePtr FindType(const Buffer& name);
	ScriptVariablePtr FindVar(const Buffer& name);
    Void GetVariableWithNode(ScriptNodePtr node, Bool right);
	Void RegisterConstant(ScriptNodePtr node);
public:
	inline Void Push(Void) {
		this->vtManager_.Push();
	}
	inline Void Pop(Void) {
		this->vtManager_.Pop();
	}
public:
	Void Evaluate(
		Vector<ScriptNodePtr>* list,
		Vector<ScriptNodePtr>* result);
	Void Trace(Void);
private:
	Void EvaluateSingleExpression(
		ScriptNodePtr left,
		ScriptObjectPtr token);
	Void EvaluateDoubleExpression(
		ScriptNodePtr left,
		ScriptNodePtr right,
		ScriptObjectPtr token);
public:
	ScriptNodePtr root = 0;
private:
    typedef struct {
        ScriptVariable var;
        ScriptObject object;
    } Temporary;
private:
	List<Temporary> tempList_;
	List<ScriptVariablePtr> varList_;
	ScriptManager vtManager_;
public:
	~ScriptPerformer();
};

class LAME_API ScriptThread {
public:
	ScriptThread(
		ScriptBuilderPtr builder,
		ScriptVirtualMachinePtr machine
	) {
		this->builder_ = builder;
		this->machine_ = machine;
	}
public:
	Void Launch(Void);
	Void Sleep(Uint32 delay);
	Void Wait(Void);
public:
	Thread thread_;
	ScriptBuilderPtr builder_ = 0;
	ScriptVirtualMachinePtr machine_ = 0;
	ScriptPerformer performer_;
};

class LAME_API ScriptBuilder {
	typedef Vector<ScriptObjectPtr>::iterator Iterator, &IteratorRef;
public:
	ScriptBuilder& Build(
		ScriptParserPtr parser,
		ScriptPerformerPtr performer);
private:
	ScriptParserPtr parser = 0;
	ScriptPerformerPtr performer = 0;
private:
	Void _Order(ScriptNodePtr node);
	Void _Build(ScriptNodePtr node, IteratorRef i);
	Void _BuildCondition(ScriptNodePtr parent, IteratorRef i);
	Void _BuildBlock(ScriptNodePtr parent, IteratorRef i);
	Void _BuildClass(ScriptNodePtr parent, IteratorRef i);
	Void _BuildField(ScriptNodePtr parent, IteratorRef i);
	Void _BuildThread(ScriptNodePtr parent, IteratorRef i);
	Bool _BuildRecursive(ScriptNodePtr parent, IteratorRef i);
	ScriptNodePtr _AppendNode(ScriptObjectPtr object);
	ScriptNodePtr _RemoveNode(ScriptNodePtr node);
	Void _PushStack(Vector<ScriptNodePtr>* stack);
	Vector<ScriptNodePtr>* _PopStack(Void);
private:
	List<Vector<ScriptNodePtr>*> stackNodeQueue_;
	Vector<ScriptNodePtr>* nodeQueue_ = 0;
	ScriptNodePtr prevNode_ = 0;
	ScriptNodePtr parentNode_ = 0;
};

class LAME_API ScriptVirtualMachine {
public:
	ScriptVirtualMachine& Execute(ScriptPerformerPtr performer);
};

class LAME_API ScriptController {
public:
	inline ScriptController& Load(StringC fileName) {
		this->parser_.Load(fileName); return *this;
	}
	inline ScriptController& Parse(StringC script) {
		this->parser_.Parse(script); return *this;
	}
	inline ScriptController& Build(Void) {
		this->builder_.Build(&this->parser_, &this->performer_); return *this;
	}
	inline ScriptController& Execute(Void) {
		this->machine_.Execute(&this->performer_); return *this;
	}
	inline ScriptController& Trace(Void) {
		this->performer_.Trace(); return *this;
	}
public:
	ScriptController() {

	}
	~ScriptController() {

	}
private:
	ScriptParser parser_;
	ScriptPerformer performer_;
	ScriptBuilder builder_;
	ScriptVirtualMachine machine_;
};

LAME_END

#endif // ~__LAME_SCRIPT__