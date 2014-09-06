#ifndef __LAME_SCRIPT_LITTLE_CALCULATOR__
#define __LAME_SCRIPT_LITTLE_CALCULATOR__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API LittleCalculator {
public:
	LittleCalculator(CodeBuilderPtr codeBuilder) :
		codeBuilder(codeBuilder)
	{
	}
public:
	Bool Compute(NodePtr node, ObjectPtr left,
		ObjectPtr right = NULL);
public:
	inline CodeBuilderPtr GetBuilder() {
		return this->codeBuilder;
	}
	inline NodePtr GetNode() {
		return this->currentNode;
	}
private:
	Bool _Unary(VariablePtr left);
	Bool _Binary(VariablePtr left, VariablePtr right);
private:
	NodePtr currentNode;
	CodeBuilderPtr codeBuilder;
};

LAME_END2

#endif // ~__LAME_SCRIPT_LITTLE_CALCULATOR__