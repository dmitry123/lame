#ifndef __LAME_SCRIPT_BUILD_LEX_CONTROLLER__
#define __LAME_SCRIPT_BUILD_LEX_CONTROLLER__

#include "BuildLex.h"

LAME_BEGIN2(Script)

class LAME_API BuildLexController {
public:
	Void RegisterLex(BuildLex::LexList lexList, BuildLex::LexProc lexProc) {
		if (lexList.size() > 0) {
			this->buildLexList.push_back(new BuildLex(lexList, lexProc));
		}
	}
	BuildLexPtrC FindLex(BuildLex::LexList lexList) {
		for (BuildLexPtrC lex : this->buildLexList) {
			if (lex->Equal(lexList)) {
				return lex;
			}
		}
		return NULL;
	}
public:
	~BuildLexController() {
		for (BuildLexPtrC lex : this->buildLexList) {
			delete lex;
		}
	}
private:
	List<BuildLexPtrC> buildLexList;
};

LAME_END2

#endif // ~__LAME_SCRIPT_BUILD_LEX_CONTROLLER__