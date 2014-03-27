#ifndef __LAME_SCRIPT__LANGUAGE__
#define __LAME_SCRIPT__LANGUAGE__

#include "Define.h"

LAME_BEGIN

typedef enum {
	kScriptLanguageDefault = -1,
	ScriptLanguageIf,
	ScriptLanguageElse,
	ScriptLanguageAmount
} ScriptLanguages;

namespace internal {
	static StringC __languageStrings[ScriptLanguageAmount] = {
		"if",
		"else"
	};
}

class ScriptLanguage {
public:
	StringC String();
	ScriptLanguage& Parse(StringC word);
public:
	inline operator ScriptLanguages () const { return this->type_; }
	inline ScriptLanguage operator = (const ScriptLanguages& type) { return ScriptLanguages(type); }
public:
	ScriptLanguage() {
		this->type_ = kScriptLanguageDefault;
	}
	ScriptLanguage(const ScriptLanguages& type) {
		this->type_ = type;
	}
private:
	ScriptLanguages type_;
};

LAME_END

#endif // ~__LAME_SCRIPT__LANGUAGE__