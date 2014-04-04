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
		{ "${object}", kScriptTypeObject }
	};
}

ScriptType& ScriptType::Parse(StringC* word) {

	this->type = kScriptTypeDefault;

	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (!strcmp(internal::scriptTypes[i].word.data(), *word)) {
			*this = internal::scriptTypes[i];
			(*word) += this->word.length();
			return *this;
		}
	}

	return *this;
}

StringC ScriptType::GetString(Void) const {
	if (this->type > kScriptTypeDefault && this->type < kScriptTypeAmount) {
		return internal::scriptTypes[this->type].word.data();
	}
	PostSyntaxError(0, "Unknown script type (%d)", this->type);
}

Bool ScriptTypeManager::Declare(const ScriptVariable& var) {

	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (!strcmp(internal::scriptTypes[i].word.data(), var.object->word.data())) {
			return LAME_FALSE;
		}
	}

	if (!this->typeMap.count(var.object->word)) {
		return LAME_FALSE;
	}

	ScriptType& t = this->typeMap[var.object->word];

	t.var = (ScriptVariablePtr)&var;
	t.var->type = t;

	return LAME_TRUE;
}

ScriptTypePtr ScriptTypeManager::Find(StringC name) {

	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (!strcmp(internal::scriptTypes[i].word.data(), name)) {
			return (ScriptTypePtr)&internal::scriptTypes[i];
		}
	}

	if (!this->typeMap.count(name)) {
		return LAME_NULL;
	}

	return &this->typeMap[name];
}

LAME_END