#include "Script.h"

#ifdef LAME_MSVC
#  define strcasecmp _stricmp
#endif

LAME_BEGIN

namespace internal {
	static const ScriptType scriptTypes[kScriptTypeAmount] = {
		{ "Void", kScriptTypeVoid, LAME_NULL },
		{ "Int", kScriptTypeInt, LAME_NULL },
		{ "Float", kScriptTypeFloat, LAME_NULL },
		{ "Bool", kScriptTypeBool, LAME_NULL },
		{ "String", kScriptTypeString, LAME_NULL },
		{ "Var", kScriptTypeVar, LAME_NULL },
		{ "${function}", kScriptTypeFunction, LAME_NULL },
		{ "${class}", kScriptTypeClass, LAME_NULL }
	};
}

Void ScriptType::Parse(StringC* word) {
	// this type may be a class, who knows
	this->type = kScriptTypeDefault;
	// look though all types
	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		// compare word
		if (!strcasecmp(internal::scriptTypes[i].name.data(), *word)) {
			// copy information from type
			*this = internal::scriptTypes[i]; 
			// move pointer
			(*word) += this->name.length();
			// exit from loop
			break;
		}
	}
}

StringC ScriptType::GetString(Void) const {
	if (this->name.length() && this->name[0] != '$') {
		return this->name.data();
	}
	// check type and return its name
	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (this->type == internal::scriptTypes[i].type) {
			return internal::scriptTypes[i].name.data();
		}
	}
	PostSyntaxError(0, "Unknown script type (%d)", this->type);
}

ScriptTypePtr ScriptType::GetList(Void) {
	return (ScriptTypePtr)&internal::scriptTypes[0];
}

LAME_END