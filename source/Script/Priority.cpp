#include "Script.h"

LAME_BEGIN

ScriptPriority& ScriptPriority::Parse(StringC word) {
	for (Uint32 i = 0; i < kPriorityAmount; i++) {
		if (!languagePriority[i]) {
			break;
		}
		if (!strncmp(word, languagePriority[i], strlen(languagePriority[i]))) {
			this->priority_ = (ScriptPriorityEnum)i;
			return *this;
		}
	}
	if (ScriptType().Parse(word) != kScriptTypeDefault) {
		this->priority_ = kPriorityType;
		return *this;
	}
	if (ScriptLanguage().Parse(word) != kScriptLanguageDefault) {
		this->priority_ = kPriorityLanguage;
		return *this;
	}
	this->priority_ = kPriorityDefault;
	return *this;
}

LAME_END