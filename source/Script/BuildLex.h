#ifndef __LAME_SCRIPT_BUILD_LEX__
#define __LAME_SCRIPT_BUILD_LEX__

#include "Lex.h"

LAME_BEGIN2(Script)

class LAME_API BuildLex {
public:
	typedef Vector<LexID> LexList;
	typedef Vector<LexNodePtr>::iterator
		Iterator;
	typedef Iterator (*LexProc)(
		NodeBuilderPtr builder,
		BuildLexPtrC lexer,
		Iterator iterator);
public:
	Bool Equal(LexList& lexList) {
		if (lexList.size() != this->lexRule.size()) {
			return FALSE;
		}
		for (Uint32 i = 0; i < lexList.size(); i++) {
			if (lexList[i] != this->lexRule[i]) {
				return FALSE;
			}
		}
		return TRUE;
	}
	Iterator Invoke(NodeBuilderPtr nodeBuilder, Iterator iterator) {
		return this->lexProc(nodeBuilder, this, iterator);
	}
public:
	BuildLex(LexList lexRule, LexProc callback) :
		lexRule(lexRule),
		lexProc(callback)
	{
	}
private:
	LexList lexRule;
	LexProc lexProc;
};

LAME_END2

#endif // ~__LAME_SCRIPT_BUILD_LEX__