#include "Type.h"

LAME_BEGIN

StringC ScriptType::String() {
	if (this->type_ > kScriptTypeDefault && this->type_ < kScriptTypeAmount) {
		return internal::__typeStrings[this->type_];
	}
	PostSyntaxError(0, "Unknown script type (%d)", this->type_);
}

ScriptType& ScriptType::Parse(StringC word) {
	for (Uint32 i = 0; i < kScriptTypeAmount; i++) {
		if (!strcmp(internal::__typeStrings[i], word)) {
			this->type_ = ScriptTypes(i); return *this;
		}
	}
	this->type_ = kScriptTypeDefault; return *this;
}

LAME_END