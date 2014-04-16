#include "Script.h"

LAME_BEGIN

Void ScriptParser::Parse(StringC script) {

	Bool isCommentLock = 0;
	Buffer constString;
	Uint32 line = 1;

	while (*script) {

		ScriptLexPtr key = new ScriptLex();
        
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
			while (*script != key->word[0]) {
				if (!*script) {
					PostSyntaxError(line, "Quote or Apostrophe mismatched", 1);
				}
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
			if (*(script - 1) == '\n' || *(script - 1) == '\r') {
				continue;
			}
			while (*script && *script != '\n') {
				++script;
			}
			continue;
		}

		if (isCommentLock) {
			continue;
		}

		if (key->word.length()) {
			this->lexList_.push_back(key);
		}
		else {
			delete key;
		}
	}
__ExitLoop:
	;
}

Void ScriptParser::Load(StringC file) {

	File handle;
	Buffer script;
    
	handle.Open(file, "rt");
	script.resize(handle.GetSize());
	handle.Read((String)script.data(), handle.GetSize());
	handle.Close();

	this->Parse(script.data());
    
//    for (ScriptLexPtr lex : this->lexList_) {
//        printf("[%s]\n", lex->word.data());
//    }
}

LAME_END