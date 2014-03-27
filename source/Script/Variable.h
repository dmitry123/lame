#ifndef __LAME_SCRIPT__VARIABLE__
#define __LAME_SCRIPT__VARIABLE__

#include "Define.h"
#include "Type.h"

LAME_BEGIN

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
	Void operator = (const ScriptVariable& value);
	Void operator + (const ScriptVariable& value);
	Void operator - (const ScriptVariable& value);
	Void operator * (const ScriptVariable& value);
	Void operator / (const ScriptVariable& value);
	Void operator % (const ScriptVariable& value);
public:
	Void operator & (const ScriptVariable& value);
	Void operator | (const ScriptVariable& value);
	Void operator ^ (const ScriptVariable& value);
	Void operator << (const ScriptVariable& value);
	Void operator >> (const ScriptVariable& value);
public:
	Void operator > (const ScriptVariable& value);
	Void operator < (const ScriptVariable& value);
	Void operator >= (const ScriptVariable& value);
	Void operator <= (const ScriptVariable& value);
	Void operator == (const ScriptVariable& value);
	Void operator != (const ScriptVariable& value);
public:
	Void operator ~ ();
	Void operator ! ();
public:
	Void operator ++ ();
	Void operator -- ();
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
		this->line = 0;
		this->boolValue = 0;
		this->floatValue = 0;
		this->intValue = 0;
		this->stringValue.clear();
		this->name.clear();
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
	Buffer name;
	ScriptType type;
	Uint32 line;
};

LAME_END

#endif // ~__LAME_SCRIPT__VARIABLE__