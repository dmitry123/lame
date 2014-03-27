#ifndef __LAME_SCRIPT__PARSER__
#define __LAME_SCRIPT__PARSER__

#include "Define.h"
#include "Variable.h"
#include "Priority.h"
#include "Language.h"

LAME_BEGIN

typedef class ScriptParser ScriptParser, *ScriptParserPtr;

typedef enum {
	kKeyWordDefault = -1,
	kKeyWordPriority,
	kKeyWordSeparator,
	kKeyWordType,
	kKeyWordLanguage,
	kKeyWordInt,
	kKeyWordFloat,
	kKeyWordString,
	kKeyWordAmount
} KeyWords;

class LAME_API KeyWord {
public:
	KeyWord& Parse(Uint32 line, StringC word);
public:
	inline KeyWord& Parse(Uint32 line, const Buffer& word) {
		return this->Parse(line, word.data());
	}
public:
	ScriptType typeValue;
	ScriptLanguage languageValue;
	ScriptNativeInt intValue;
	ScriptNativeFloat floatValue;
	ScriptNativeString stringValue;
public:
	ScriptPriorities priority;
	KeyWords key;
	Buffer word;
	Uint32 line;
};

class LAME_API ScriptParser {
public:
	Void Parse(StringC script);
public:
	inline Void Parse(const Buffer& script) {
		this->Parse(script.data());
	}
private:
	List <KeyWord> keyQueue;
};

LAME_END

#endif // ~__LAME_SCRIPT__PARSER__