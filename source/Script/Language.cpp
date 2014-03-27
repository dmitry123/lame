#include "Language.h"

LAME_BEGIN

StringC ScriptLanguage::String() {
	if (this->type_ > kScriptLanguageDefault && this->type_ < ScriptLanguageAmount) {
		return internal::__languageStrings[this->type_];
	}
	PostSyntaxError(0, "Unknown language construction (%d)", this->type_);
}

ScriptLanguage& ScriptLanguage::Parse(StringC word) {
	for (Uint32 i = 0; i < ScriptLanguageAmount; i++) {
		if (!strcmp(internal::__languageStrings[i], word)) {
			this->type_ = ScriptLanguages(i); return *this;
		}
	}
	this->type_ = kScriptLanguageDefault; return *this;
}

LAME_END