#ifndef __LAME_SCRIPT__TYPE__
#define __LAME_SCRIPT__TYPE__

#include "Define.h"

LAME_BEGIN

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
		"int",
		"float",
		"bool",
		"string",
		"void"
	};
}

class ScriptType {
public:
	StringC String();
	ScriptType& Parse(StringC word);
public:
	inline operator ScriptTypes () const { return this->type_; }
	inline ScriptType operator = (const ScriptTypes& type) { return ScriptTypes(type); }
public:
	ScriptType() { 
		this->type_ = kScriptTypeDefault;
	}
	ScriptType(const ScriptTypes& type) {
		this->type_ = type;
	}
private:
	ScriptTypes type_;
};

typedef Sint32 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Bool ScriptNativeBool;
typedef Buffer ScriptNativeString;

LAME_END

#endif // ~__LAME_SCRIPT__TYPE__