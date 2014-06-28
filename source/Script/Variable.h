#ifndef __LAME_SCRIPT__VARIABLE__
#define __LAME_SCRIPT__VARIABLE__

#include "Object.h"
#include "Object.h"
#include "Class.h"

LAME_BEGIN2(Script)

class LAME_API ScriptVar : public ScriptObject {
public:
	enum class Var {
		Integer,
		Float,
		Object
	};
public:
	const ScriptClassPtr classType;
public:
	ScriptVar(BufferRefC name, ScriptObjectPtrC type);
	~ScriptVar();
public:
	ScriptVarPtr MakeInteger(ScriptNativeInt i);
	ScriptVarPtr MakeFloat(ScriptNativeFloat f);
	ScriptVarPtr MakeObject(ScriptClassPtr c);
	ScriptVarPtr MakeInteger(Void);
	ScriptVarPtr MakeFloat(Void);
	ScriptVarPtr MakeObject(Void);
public:
	ScriptError Clone(ScriptObjectPtrC object) override;
	ScriptError Cast(ScriptObjectPtrC object) override;
	Void Trace(Uint32 offset) override;
public:
	inline ScriptClassPtr GetClass() override { return this->classType; }
	inline ScriptVarPtr GetVariable() override { return this; }
public:
	inline ScriptClassPtr GetObject() {
		return this->objectValue;
	}
public:
	union {
		ScriptNativeInt intValue;
		ScriptNativeFloat floatValue;
	} v;
public:
	ScriptClassPtr objectValue;
private:
	Var varType;
};

LAME_END2

#endif // ~__LAME_SCRIPT__VARIABLE__