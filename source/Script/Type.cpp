#include "Script.h"

LAME_BEGIN

namespace internal {
	static const ScriptType scriptTypes[kScriptTypeAmount] = {
		{ "void", kScriptTypeVoid },
		{ "int", kScriptTypeInt },
		{ "float", kScriptTypeFloat },
		{ "bool", kScriptTypeBool },
		{ "string", kScriptTypeString },
		{ "${function}", kScriptTypeFunction },
		{ "${class}", kScriptTypeClass }
	};
}

Void ScriptType::Parse(StringC* word) {
	// this type may be a class, who knows
	this->type = kScriptTypeDefault;
	// look though all types
	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		// compare word
		if (!strcmp(internal::scriptTypes[i].name.data(), *word)) {
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
	// check type and return its name
	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (this->type == internal::scriptTypes[i].type) {
			return internal::scriptTypes[i].name.data();
		}
	}
	PostSyntaxError(0, "Unknown script type (%d)", this->type);
}

LAME_END