#ifndef __LAME_SCRIPT__VARIABLE__
#define __LAME_SCRIPT__VARIABLE__

#include "Object.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API Variable : public Object {
	friend class Array;
public:
	enum class Var {
		Integer,
		Float,
		Object,
		String
	};
public:
	const ClassPtr classType;
public:
	Variable(BufferRefC name, ObjectPtrC classType, NodePtr node = NULL);
	~Variable();
public:
	VariablePtr SetInteger(ScriptNativeInt i);
	VariablePtr SetFloat(ScriptNativeFloat f);
	VariablePtr SetObject(ClassPtr c);
	VariablePtr SetString(ScriptNativeString s);
public:
	ScriptNativeInt GetInteger(Void);
	ScriptNativeFloat GetFloat(Void);
	ScriptNativeString GetString(Void);
public:
	Error Clone(ObjectPtrC object) override;
	Void Trace(Uint32 offset) override;
public:
	inline ClassPtr GetClass() override { return this->classType; }
	inline VariablePtr GetVariable() override { return this; }
public:
	inline ClassPtr GetObject() {
		return this->objectValue;
	}
	inline Var GetVarType() {
		return varType;
	}
public:
	union {
		ScriptNativeInt intValue;
		ScriptNativeFloat floatValue;
	} v;
public:
	ClassPtr objectValue;
	ScriptNativeString stringValue;
public:
	Bool IsChar(Void);
	Bool IsByte(Void);
	Bool IsShort(Void);
	Bool IsInt(Void);
	Bool IsLong(Void);
	Bool IsFloat(Void);
	Bool IsDouble(Void);
	Bool IsObject(Void);
	Bool IsClass(Void);
	Bool IsBoolean(Void);
	Bool IsVoid(Void);
	Bool IsString(Void);
private:
	Variable(
		BufferRefC name,
		ObjectPtrC classType,
		Type type,
		NodePtr node = NULL);
private:
	Var varType;
};

LAME_END2

#endif // ~__LAME_SCRIPT__VARIABLE__