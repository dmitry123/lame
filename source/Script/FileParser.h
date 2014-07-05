#ifndef __LAME_SCRIPT__PARSER_PARSER__
#define __LAME_SCRIPT__PARSER_PARSER__

#include "LexNode.h"

LAME_BEGIN2(Script)

class LAME_API FileParser {
public:
	Void Load(StringC fileName);
	Void Parse(StringC script);
public:
	inline Vector<LexNodePtr>& GetLexList() {
		return this->lexList;
	}
private:
	Vector<LexNodePtr> lexList;
};

LAME_END2

#endif // ~__LAME_SCRIPT__PARSER_PARSER__