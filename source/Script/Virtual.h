#ifndef __LAME_SCRIPT__VIRTUAL__
#define __LAME_SCRIPT__VIRTUAL__

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
} ScriptType;

namespace internal {
	static StringC __typeStrings[kScriptTypeAmount] = {
		"Int",
		"Float",
		"Bool",
		"String",
		"Void"
	};
}

inline StringC StringType(ScriptType type) {
	if (type > kScriptTypeDefault && type < kScriptTypeAmount) {
		return internal::__typeStrings[type];
	}
	return "${Unknown}";
}

typedef Sint32 ScriptNativeInt;
typedef Float32 ScriptNativeFloat;
typedef Bool ScriptNativeBool;
typedef Buffer ScriptNativeString;

class IScriptType {
public:
	inline Void Declare(ScriptType type) {
		this->type_ = type;
	}
	inline Bool Parse(StringC word) {
		for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
			if (!strcmp(internal::__typeStrings[i], word)) {
				this->type_ = ScriptType(i);
				return LAME_TRUE;
			}
		}
		this->type_ = kScriptTypeDefault;
		return LAME_FALSE;
	}
public:
	IScriptType() {
		this->type_ = kScriptTypeDefault;
	}
public:
	ScriptType type_;
	ScriptNativeInt int_;
	ScriptNativeFloat float_;
	ScriptNativeBool bool_;
	ScriptNativeString string_;
};

LAME_END

#endif // ~__LAME_SCRIPT__VIRTUAL__