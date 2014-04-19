#ifndef __LAME_SCRIPT__MANAGER__
#define __LAME_SCRIPT__MANAGER__

#include "Define.h"
#include "Exception.h"

LAME_BEGIN

typedef enum {
	kScriptTypeDefault,
	kScriptTypeVoid,
	kScriptTypeInt,
	kScriptTypeBool,
	kScriptTypeFloat,
	kScriptTypeString,
	kScriptTypeClass,
	kScriptTypeArray,
	kScriptTypeList,
	kScriptTypeFunction,
	kScriptTypeAuto
} ScriptTypeID;

typedef enum {
	kScriptFlagDefault = 0x0000,
	kScriptFlagTemp = 0x0001,
	kScriptFlagType = 0x0002
} ScriptFlagID;

class LAME_API ScriptVar {
	friend ScriptPerformer;
public:
	ScriptTypeID type;
	Buffer name;
public:
	ScriptVar(ScriptTypeID type, BufferRefC name, BufferRefC typeName, Uint32 line) :
		type(type),
		name(name),
		typeName(typeName),
		line(line)
	{
		this->Reset();
		if (name == typeName) {
			this->MakeType();
		}
	}
public:
	Void Reset(Void);
	Void Convert(ScriptVarPtr type);
	Void Clone(ScriptVarPtr result);
public:
	inline Bool IsDefault() const { return this->type == kScriptTypeDefault; }
	inline Bool IsVoid() const { return this->type == kScriptTypeVoid; }
	inline Bool IsInt() const { return this->type == kScriptTypeInt; }
	inline Bool IsBool() const { return this->type == kScriptTypeBool; }
	inline Bool IsFloat() const { return this->type == kScriptTypeFloat; }
	inline Bool IsString() const { return this->type == kScriptTypeString; }
	inline Bool IsClass() const { return this->type == kScriptTypeClass; }
	inline Bool IsArray() const { return this->type == kScriptTypeArray; }
	inline Bool IsFunction() const { return this->type == kScriptTypeFunction; }
	inline Bool IsAuto() const { return this->type == kScriptTypeAuto; }
public:
	inline Bool IsTemp() const { return (this->flags & kScriptFlagTemp) != 0; }
	inline Bool IsType() const { return (this->flags & kScriptFlagType) != 0; }
public:
	inline Void MakeTemp() { this->flags |= kScriptFlagTemp; }
	inline Void MakeType() { this->flags |= kScriptFlagType; }
private:
	Void _EvaluateMath(ScriptVarPtr right, ScriptLexPtr lex);
	Void _EvaluateBool(ScriptVarPtr right, ScriptLexPtr lex);
private:
	ScriptNodePtr _EvaluateDouble(ScriptNodePtr right, ScriptLexPtr lex);
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
	ScriptCallback callback;
	Buffer typeName;
	Uint32 line;
};

class LAME_API ScriptManager {
	friend ScriptPerformer;
public:
	typedef Map<Buffer, ScriptVarPtr> Map, *MapPtr;
public:
	Bool Declare(ScriptVarPtr var);
	ScriptVarPtr Find(BufferRefC varName);
public:
	Void Push(Void);
	Void Pop(Void);
public:
	ScriptTypeID GetTypeID(BufferRefC typeName);
public:
	ScriptManager(Void);
	~ScriptManager(Void);
private:
	MapPtr varMap = 0;
	MapPtr typeMap = 0;
private:
	List<Map> varMapQueue;
	List<Map> typeMapQueue;
};

LAME_END

#endif // ~__LAME_SCRIPT__MANAGER__