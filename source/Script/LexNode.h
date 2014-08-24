#ifndef __LAME_SCRIPT__PARSER_LEX_NODE__
#define __LAME_SCRIPT__PARSER_LEX_NODE__

#include "Lex.h"

LAME_BEGIN2(Script)

class LAME_API LexNode {
public:
	const Buffer word;
	const Uint32 line;
	      LexPtr lex;
public:
	LexNode(BufferRefC word, Uint32 line, LexPtrC lex) :
		word(word),
		line(line),
		lex(lex)
	{
		this->priority = lex ? lex->priority : 0;
		this->args = lex ? lex->args : 0;
	}
public:
	Void ChangeLex(LexPtrC lex) {
		this->lex = lex;
	}
public:
	Uint32 priority;
	Uint32 args;
};

LAME_END2

#endif // ~__LAME_SCRIPT__PARSER_LEX_NODE__