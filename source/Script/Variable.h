#ifndef __LAME_SCRIPT__VARIABLE__
#define __LAME_SCRIPT__VARIABLE__

#include "Object.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API Variable : public Object {
	friend class Array;
public:
	enum class Var {
		Integer, /* char, byte, short, int, long */
		Float,   /* float, double */
        Boolean, /* boolean */
		Object,  /* Object, Class ... */
		String,  /* String */
		Array    /* char[], byte[], String[] ... */
	};
public:
	ClassPtr classType;
	ObjectPtr registerType;
    Bool wasInStack;
public:
	Variable(BufferRefC name, ScopePtr parent, ClassPtr classType, Bool isArray = FALSE);
	~Variable();
public:
	VariablePtr SetInteger(ScriptNativeInt i);
	VariablePtr SetFloat(ScriptNativeFloat f);
	VariablePtr SetObject(ClassPtr c);
	VariablePtr SetString(ScriptNativeString s);
    VariablePtr SetBoolean(ScriptNativeBool b);
public:
	ScriptNativeInt GetInteger(Void);
	ScriptNativeFloat GetFloat(Void);
    
    ScriptNativeBool GetBoolean(Void);
public:
	Bool Equal(ObjectPtrC object) override;
	ObjectPtr Clone(BufferRefC name, ObjectPtr parent) override;
	Void Trace(Uint32 offset) override;
	HashType Hash(Void) override;
	Uint32 Size(Void) override;
	ClassPtr GetClass() final override;
	VariablePtr GetVariable() override;
public:
	inline ClassPtr GetClassType() { return this->classType; }
	inline ClassPtr GetObject() { return this->objectValue; }
	inline Var GetVarType() { return this->varType; }
public:
	inline Void SetClassType(ClassPtr classType) {
		this->classType = classType;
	}
public:
	union {
		ScriptNativeInt intValue;
		ScriptNativeFloat floatValue;
	} v;
public:
	ClassPtr objectValue;
	ScriptNativeString stringValue;
private:
	Var varType;
};

LAME_END2

#endif // ~__LAME_SCRIPT__VARIABLE__