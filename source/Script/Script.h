#ifndef __LAME_SCRIPT__
#define __LAME_SCRIPT__

#include "Define.h"
#include "Exception.h"

#include <memory>

LAME_BEGIN

typedef enum {
	kScriptObjectDefault = -1,
    // variable/constants
	kScriptObjectVariable,
	kScriptObjectInt,
	kScriptObjectFloat,
	kScriptObjectString,
	kScriptObjectBool,
    // brackets
	kScriptObjectBracketL,
	kScriptObjectBracketR,
	kScriptObjectParentheseL,
	kScriptObjectParentheseR,
    // selectors
	kScriptObjectDirect,
	kScriptObjectMediated,
    // single-argument
	kScriptObjectIncrement,
	kScriptObjectDecrement,
	kScriptObjectSizeof,
	kScriptObjectBitNot,
	kScriptObjectNot,
	kScriptObjectUnaryMinus,
	kScriptObjectUnaryPlus,
	kScriptObjectType,
    kScriptObjectNew,
    // constructions
	kScriptObjectIf,
	kScriptObjectElse,
	kScriptObjectWhile,
	kScriptObjectDo,
	kScriptObjectFor,
	kScriptObjectFunction,
    // math
	kScriptObjectMul,
	kScriptObjectDiv,
	kScriptObjectMod,
	kScriptObjectAdd,
	kScriptObjectSub,
	kScriptObjectBitShiftL,
	kScriptObjectBitShiftR,
	kScriptObjectBitAnd,
	kScriptObjectBitXor,
	kScriptObjectBitOr,
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
    // bool
	kScriptObjectBellow,
	kScriptObjectAbove,
	kScriptObjectBellowEqual,
	kScriptObjectAboveEqual,
	kScriptObjectEqual,
	kScriptObjectNotEqual,
	kScriptObjectAnd,
	kScriptObjectOr,
    // special
	kScriptObjectComa,
	kScriptObjectCommentL,
	kScriptObjectCommentR,
	kScriptObjectLineComment,
	kScriptObjectQuote,
	kScriptObjectApostrophe,
	kScriptObjectSemicolon,
	kScriptObjectBraceL,
	kScriptObjectBraceR,
    // modificators
	kScriptObjectPublic,
	kScriptObjectPrivate,
	kScriptObjectProtected,
	kScriptObjectFinal,
	kScriptObjectOverride,
	kScriptObjectAbstract,
	kScriptObjectStatic,
    // keywords
	kScriptObjectClass,
	kScriptObjectImplements,
	kScriptObjectExtends,
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
	kScriptFlagDefault = 0x00001,
	kScriptFlagAbstract = 0x00002,
	kScriptFlagFinal = 0x00004,
	kScriptFlagOverride = 0x00008,
	kScriptFlagPublic = 0x00010,
	kScriptFlagPrivate = 0x00020,
	kScriptFlagProtected = 0x00040,
	kScriptFlagStatic = 0x00080,
    kScriptFlagVar = 0x00100,
	kScriptFlagFunction = 0x00200,
	kScriptFlagVoid = 0x00400,
	kScriptFlagInt = 0x00800,
	kScriptFlagFloat = 0x01000,
	kScriptFlagBool = 0x02000,
	kScriptFlagString = 0x04000,
	kScriptFlagClass = 0x08000,
    kScriptFlagTemp = 0x10000
} EScriptFlag;

typedef enum {
	kScriptNodeDefault = -1,
	kScriptNodeCondition,
	kScriptNodeClass,
	kScriptNodeField,
	kScriptNodeThread,
	kScriptNodeDeclare,
	kScriptNodeAmount
} EScriptNode;

/* Basic language types */

typedef Sint64 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Buffer ScriptNativeString;
typedef Bool ScriptNativeBool;

/* Script objects */
;
typedef class ScriptLex *ScriptLexPtr;
typedef class ScriptNode *ScriptNodePtr;
typedef class ScriptVar *ScriptVarPtr;
typedef class ScriptManager *ScriptManagerPtr;
typedef class ScriptParser *ScriptParserPtr;
typedef class ScriptBuilder *ScriptBuilderPtr;
typedef class ScriptVirtualMachine *ScriptVirtualMachinePtr;

/* Callback type for native invokes */

typedef Void(*ScriptCallback)(ScriptNodePtr var, VoidP data);

/* Native class type */

typedef std::shared_ptr<ScriptNode> ScriptNativeClass;
typedef const Buffer& BufferRefC;

class LAME_API ScriptLex {
public:
	Void Parse(StringC* word);
	StringC Parse(StringC word);
	Void Reset(Void);
public:
	Buffer word;
	Uint32 priority;
	EScriptObject object;
	Uint32 arguments;
	EScriptAssociativity associativity;
	Uint32 line;
public:
	inline Bool IsLeftAssociated() const { return this->associativity == kScriptAssociativityLeft; }
	inline Bool IsRightAssociated() const { return this->associativity == kScriptAssociativityRight; }
	inline Bool IsType() const { return this->object == kScriptObjectType; }
	inline Bool IsClass() const { return this->object == kScriptObjectClass; }
	inline Bool IsCondition() const { return this->object >= kScriptObjectIf && this->object <= kScriptObjectFor; }
	inline Bool IsVariable() const { return this->object == kScriptObjectVariable || this->object == kScriptObjectDefault; }
	inline Bool IsModificator() const { return this->object >= kScriptObjectPublic && this->object <= kScriptObjectStatic; }
	inline Bool IsThread() const { return this->object == kScriptObjectThread; }
    inline Bool IsMath() const { return this->object >= kScriptObjectMul && this->object <= kScriptObjectBitOrSet; }
    inline Bool IsBool() const { return this->object >= kScriptObjectBellow && this->object <= kScriptObjectOr; }
	inline Bool IsFunction() const { return this->object == kScriptObjectFunction; }
public:
	static ScriptLexPtr Find(EScriptObject object);
};

class LAME_API ScriptNode {
	typedef Vector<ScriptNodePtr> NodeList, *NodeListPtr;
public:
	ScriptLexPtr __lex;
public:
	Void Reset(Void);
	Void Order(Void);
public:
	inline ScriptNodePtr FindField(BufferRefC name) { return this->_Find(&this->fieldList, name); }
	inline ScriptNodePtr FindCondition(BufferRefC name) { return this->_Find(&this->conditionList, name); }
	inline ScriptNodePtr FindBlock(BufferRefC name) { return this->_Find(&this->blockList, name); }
public:
	inline Void RegisterField(ScriptNodePtr node) { this->fieldList.push_back(node); }
	inline Void RegisterCondition(ScriptNodePtr node) { this->conditionList.push_back(node); }
	inline Void RegisterBlock(ScriptNodePtr node) { this->blockList.push_back(node); }
public:
	Void GoToLeft(Void);
public:
	ScriptNodePtr next = 0;
	ScriptNodePtr prev = 0;
	ScriptNodePtr parent = 0;
public:
	Vector<ScriptNodePtr> conditionList;
	Vector<ScriptNodePtr> blockList;
	Vector<ScriptNodePtr> fieldList;
public:
	ScriptVarPtr var = 0;
	EScriptNode node = kScriptNodeDefault;
private:
	ScriptNodePtr _Find(NodeListPtr list, BufferRefC name);
	Void _Order(NodeListPtr root);
public:
	~ScriptNode();
};

class LAME_API ScriptVar {
	friend class ScriptManager;
    friend class ScriptVirtualMachine;
public:
	typedef enum {
		kScriptTypeDefault,
		kScriptTypeVar,
		kScriptTypeFunction,
		kScriptTypeVoid,
		kScriptTypeInt,
		kScriptTypeFloat,
		kScriptTypeBool,
		kScriptTypeString,
		kScriptTypeClass
	} EType;
private:
	ScriptVar(Void);
public:
	ScriptVar(ScriptLexPtr lex) : ScriptVar() {
		this->lex = lex;
	}
public:
	ScriptVar(ScriptNativeInt intValue);
	ScriptVar(ScriptNativeFloat floatValue);
	ScriptVar(ScriptNativeBool intValue);
	ScriptVar(ScriptNativeString stringValue);
	ScriptVar(ScriptNativeClass classValue);
public:
	Void Reset(Void);
	Void Convert(EType& type);
	Void Clone(ScriptVarPtr result);
	Void Invoke(Vector<ScriptNodePtr>& parameters);
public:
	inline Bool IsAbstract() const { return (this->flags & kScriptFlagAbstract) != 0; }
	inline Bool IsFinal() const { return (this->flags & kScriptFlagFinal) != 0; }
	inline Bool IsOverride() const { return (this->flags & kScriptFlagOverride) != 0; }
	inline Bool IsPublic() const { return (this->flags & kScriptFlagPublic) != 0; }
	inline Bool IsPrivate() const { return (this->flags & kScriptFlagPrivate) != 0; }
	inline Bool IsProtected() const { return (this->flags & kScriptFlagProtected) != 0; }
	inline Bool IsStatic() const { return (this->flags & kScriptFlagStatic) != 0; }
	inline Bool IsVar() const { return (this->flags & kScriptFlagVar) != 0; }
	inline Bool IsFunction() const { return (this->flags & kScriptFlagFunction) != 0; }
	inline Bool IsVoid() const { return (this->flags & kScriptFlagVoid) != 0; }
	inline Bool IsInt() const { return (this->flags & kScriptFlagInt) != 0; }
	inline Bool IsFloat() const { return (this->flags & kScriptFlagFloat) != 0; }
	inline Bool IsBool() const { return (this->flags & kScriptFlagBool) != 0; }
	inline Bool IsString() const { return (this->flags & kScriptFlagString) != 0; }
	inline Bool IsClass() const { return (this->flags & kScriptFlagClass) != 0; }
	inline Bool IsTemp() const { return (this->flags & kScriptFlagTemp) != 0; }
public:
	inline Void MakeVar() {
		this->flags |= kScriptFlagVar;
		this->type = kScriptTypeVar;
	}
	inline Void MakeFunction() {
		this->flags |= kScriptFlagFunction;
		this->type = kScriptTypeFunction;
	}
	inline Void MakeVoid() {
		this->flags |= kScriptFlagVoid;
		this->type = kScriptTypeVoid;
	}
	inline Void MakeInt() {
		this->flags |= kScriptFlagInt;
		this->type = kScriptTypeInt;
	}
	inline Void MakeFloat() {
		this->flags |= kScriptFlagFloat;
		this->type = kScriptTypeFloat;
	}
	inline Void MakeBool() {
		this->flags |= kScriptFlagBool;
		this->type = kScriptTypeBool;
	}
	inline Void MakeString() {
		this->flags |= kScriptFlagString;
		this->type = kScriptTypeString;
	}
	inline Void MakeClass() {
		this->flags |= kScriptFlagClass;
		this->type = kScriptTypeClass;
	}
    inline Void MakeTemp() {
        this->flags |= kScriptFlagTemp;
    }
private:
	Void _EvaluateMath(ScriptVarPtr right, ScriptLexPtr lex);
	Void _EvaluateBool(ScriptVarPtr right, ScriptLexPtr lex);
private:
    ScriptVarPtr _EvaluateDouble(ScriptNodePtr right, ScriptLexPtr lex);
	ScriptVarPtr _EvaluateSingle(ScriptLexPtr lex);
public:
	Bool Boolean(Void);
public:
	ScriptNativeInt intValue;
	ScriptNativeFloat floatValue;
	ScriptNativeString stringValue;
	ScriptNativeClass classValue;
public:
	Uint32 flags;
	EType type;
	Buffer name;
	ScriptCallback callback;
	ScriptLexPtr lex;
    ScriptNodePtr condition;
    Bool result;
	Buffer className;
};

class LAME_API ScriptManager {
	friend class ScriptPerformer;
    friend class ScriptVirtualMachine;
public:
	typedef Map<Buffer, ScriptVarPtr> Map, *MapPtr;
public:
	inline Bool DeclareType(ScriptVarPtr var) {
		// declare type
		return this->_Declare(this->typeMap, var);
	}
	inline ScriptVarPtr FindType(const Buffer& varName) {
		return this->_Find(this->typeMap, varName);
	}
	inline Bool DeclareVar(ScriptVarPtr var) {
		// reset class valur for variable
		var->classValue.reset();
		// declare variable
		return this->_Declare(this->varMap, var);
	}
	inline ScriptVarPtr FindVar(const Buffer& varName) {
		return this->_Find(this->varMap, varName);
	}
public:
	Void Push(Void);
	Void Pop(Void);
public:
	ScriptManager(Void);
	~ScriptManager(Void);
private:
	Bool _Declare(MapPtr map, ScriptVarPtr var);
	ScriptVarPtr _FindQueue(List<Map>* queue, const Buffer& name);
	ScriptVarPtr _Find(MapPtr map, const Buffer& name);
private:
	MapPtr varMap = 0;
	MapPtr typeMap = 0;
private:
	List<Map> varMapQueue;
	List<Map> typeMapQueue;
};

class LAME_API ScriptParser {
	friend class ScriptBuilder;
	friend class ScriptVirtualMachine;
public:
	Void Parse(StringC script);
	Void Load(StringC file);
private:
	Vector<ScriptLexPtr> lexList_;
public:
	~ScriptParser() {
		for (ScriptLexPtr object : this->lexList_) {
			delete object;
		}
		this->lexList_.clear();
	}
};

class LAME_API ScriptBuilder {
	typedef Vector<ScriptLexPtr>::iterator
		 Iterator,
		&IteratorRef;
public:
	Void Build(ScriptParserPtr parser, ScriptVirtualMachinePtr machine);
private:
	Void _Order(ScriptNodePtr node);
	Bool _RecursiveBuild(ScriptNodePtr node, IteratorRef i);
private:
	Void _BuildCondition(ScriptNodePtr node, IteratorRef i);
	Void _BuildBlock(ScriptNodePtr node, IteratorRef i);
	Void _BuildClass(ScriptNodePtr node, IteratorRef i);
	Void _BuildAttribute(ScriptNodePtr node, IteratorRef i);
	Void _BuildThread(ScriptNodePtr node, IteratorRef i);
private:
	ScriptNodePtr _CreateNode(ScriptLexPtr lex);
	ScriptNodePtr _RemoveNode(ScriptNodePtr node);
private:
	Void _Push(Vector<ScriptNodePtr>* stack);
	Vector<ScriptNodePtr>* _Pop(Void);
private:
	ScriptParserPtr parser = 0;
	ScriptVirtualMachinePtr machine = 0;
	List<Vector<ScriptNodePtr>*> stackNodeQueue_;
	Vector<ScriptNodePtr>* nodeQueue_ = 0;
	ScriptNodePtr prevNode_ = 0;
	ScriptNodePtr parentNode_ = 0;
	Vector<ScriptNodePtr> nodeList_;
public:
	~ScriptBuilder() {
		for (ScriptNodePtr n : this->nodeList_) {
			delete n;
		}
		this->nodeList_.clear();
	}
};

class LAME_API ScriptVirtualMachine {
	friend class ScriptBuilder;
public:
    Void Execute(Void);
    Void Trace(Void);
private:
	Void _DeclareType(ScriptNodePtr node);
	Void _DeclareVar(ScriptNodePtr node);
	ScriptVarPtr _FindType(const Buffer& name);
	ScriptVarPtr _FindVar(const Buffer& name);
	Void _RegisterConstant(ScriptNodePtr node);
    Void _Evaluate(Vector<ScriptNodePtr>* list, Vector<ScriptNodePtr>* result);
    Void _EvaluateCondition(ScriptNodePtr node);
    Void _Trace(ScriptNodePtr node);
    StringC _GetTypeName(Uint32 type);
private:
    ScriptNodePtr _CreateTemp(ScriptNodePtr node);
private:
	ScriptNodePtr root = 0;
	ScriptManager manager;
private:
    typedef struct {
        ScriptNode node;
        ScriptVar var;
    } Temporary;
private:
    Uint32 traceDepth_ = 0;
	Vector<ScriptVarPtr> varList_;
	List<Temporary> tempList_;
public:
	~ScriptVirtualMachine() {
		for (ScriptVarPtr var : this->varList_) {
			delete var;
		}
		this->varList_.clear();
	}
};

LAME_END

#endif // ~__LAME_SCRIPT__