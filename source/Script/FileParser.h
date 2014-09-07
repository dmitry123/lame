#ifndef __LAME_SCRIPT__PARSER_PARSER__
#define __LAME_SCRIPT__PARSER_PARSER__

#include "LexNode.h"

LAME_BEGIN2(Script)

class LAME_API FileParser {
public:
	inline Void Load(BufferRefC fileName) {
		this->Load(fileName.data());
	}
	inline Void Parse(BufferRefC script) {
		this->Parse(script.data());
	}
public:
	Void Load(StringC fileName);
	Void Parse(StringC script);
	Void Clear(Void);
public:
	static Bool IsDigit(Sint8 symbol);
	static Bool IsLetter(Sint8 symbol);
	static Bool IsFloatValue(StringC string);
	static Bool IsFloatValue(BufferRefC string);
	static Bool IsIntValue(StringC string);
	static Bool IsHexValue(StringC string);
	static Bool IsStringValue(StringC string);
	static Bool IsIntValue(BufferRefC string);
public:
	static ScriptNativeFloat ParseFloatValue(StringC string);
	static ScriptNativeString ParseStringValue(StringC string);
	static ScriptNativeInt ParseIntValue(StringC string);
public:
	inline Deque<LexNodePtr>& GetLexList() {
		return this->lexList;
	}
private:
	Deque<LexNodePtr> lexList;
	Buffer fileText;
};

LAME_END2

#endif // ~__LAME_SCRIPT__PARSER_PARSER__