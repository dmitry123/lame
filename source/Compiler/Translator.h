#ifndef __LAME_SCRIPT__PERFORMER_PERFORMER__
#define __LAME_SCRIPT__PERFORMER_PERFORMER__

#include "Define.h"

LAME_BEGIN2(Compiler)

using namespace Script;

class LAME_API CodeTranslator {
public:
	typedef       Vector<NodePtr>  NodeList;
	typedef       Vector<NodePtr>& NodeListRef;
	typedef const Vector<NodePtr>& NodeListRefC;
	typedef       Vector<ObjectPtr>  VarList;
	typedef       Vector<ObjectPtr>& VarListRef;
	typedef const Vector<ObjectPtr>& VarListRefC;
public:
	CodeTranslator() :
		builder(0)
	{
	}
public:
	~CodeTranslator() {
	}
public:
	Void Overload(Void);
	Void Translate(NodeBuilderPtr builder);
	Bool Evaluate(NodeListRef nodeList);
private:
	typedef struct {
		NodePtr node;
		ObjectPtr var;
	} VarNode, *VarNodePtr;
private:
	Void _Translate0(NodePtr node);
	Void _Translate1(NodePtr node);
	Void _Translate2(NodePtr node);
	Void _TranslateNew(NodePtr node);
	Void _TranslateCondition(NodePtr node);
	Void _ReadArguments(NodePtr node);
private:
	Vector<VarNode> varStack;
	NodeBuilderPtr builder;
	Vector<VarNode> argsArray;
	Uint32 methodHash;
};


LAME_END2

#endif // ~__LAME_SCRIPT__PERFORMER_PERFORMER__