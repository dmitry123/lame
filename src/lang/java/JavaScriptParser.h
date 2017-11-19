#ifndef __LAME_LANG__JAVA_SCRIPT_PARSER__
#define __LAME_LANG__JAVA_SCRIPT_PARSER__

#include "../AbstractLoader.h"
#include "../ScriptParser.h"

LAME_LANG_BEGIN_WITH(java)

typedef class ScriptParser : public ::lame::lang::ScriptParser {
	typedef ::lame::lang::ScriptParser Parent;
public:
	ScriptParser(AbstractLoaderPtr loader, TokenFinderPtr finder):
		Parent(loader, finder)
	{
	}
	ScriptParser(const StringPtr code, TokenFinderPtr finder):
		Parent(code, finder)
	{
	}
} *ScriptParserPtr;

LAME_LANG_END_WITH

#endif // ~__LAME_LANG__JAVA_SCRIPT_PARSER__