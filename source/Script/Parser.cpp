#include "Script.h"

LAME_BEGIN

ScriptParser& ScriptParser::Parse(StringC script) {

	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;

	while (*script) {

		ScriptObjectPtr key = new ScriptObject();

		key->Parse(&script);

		while (
			*(script) == ' ' ||
			*(script) == '\t' ||
			*(script) == '\r' ||
			*(script) == '\n'
		) {
			if (!*script) {
				goto __ExitLoop;
			}
			if (*script == '\n' || *script == '\r') {
				++line;
			}
			script++;
		}

		key->line = line;

		if (key->object == kScriptObjectQuote || key->object == kScriptObjectApostrophe) {
			constString.clear();
			while (*script && *script != *key->GetString()) {
				constString.append(1, *script++);
			} ++script;
			key->object = kScriptObjectString;
			key->word = constString;
			key->associativity = kScriptAssociativityRight;
		}

		if (key->object == kScriptObjectCommentL) {
			isCommentLock = LAME_TRUE; continue;
		}
		else if (key->object == kScriptObjectCommentR) {
			isCommentLock = LAME_FALSE; continue;
		}

		if (key->object == kScriptObjectLineComment) {
			while (*script && *script != '\n') {
				++script;
			}
			continue;
		}

		if (isCommentLock) {
			continue;
		}

		if (key->word.length()) {
			this->objectList.push_back(key);
		}
	}
__ExitLoop:
	return *this;
}

ScriptParser& ScriptParser::Load(StringC file) {

	File handle;
	Buffer script;

	handle.Open(file, "rt");
	script.resize(handle.GetSize());
	handle.Read((String)script.data(), handle.GetSize());
	handle.Close();

	this->Parse(script.data());

	return *this;
}

LAME_END