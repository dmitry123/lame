#ifndef __LAME_SCRIPT__PERFORMER_PERFORMER__
#define __LAME_SCRIPT__PERFORMER_PERFORMER__

#include "Define.h"

LAME_BEGIN2(Compiler)

using namespace Script;

class LAME_API NodePerformer {
public:
	typedef Void(*ForEachNode)(NodePtr node);
public:
	typedef       Vector<NodePtr>  NodeList;
	typedef       Vector<NodePtr>& NodeListRef;
	typedef const Vector<NodePtr>& NodeListRefC;
	typedef       Vector<ObjectPtr>  VarList;
	typedef       Vector<ObjectPtr>& VarListRef;
	typedef const Vector<ObjectPtr>& VarListRefC;
public:
	NodePerformer() :
		builder(0)
	{
	}
public:
	~NodePerformer() {
	}
public:
	Void Overload(Void);
	Void Perform(NodeBuilderPtr builder);
	Bool Evaluate(NodeListRef nodeList);
private:
	typedef struct {
		NodePtr node;
		ObjectPtr var;
	} VarNode, *VarNodePtr;
private:
	Void _TranslateNew(NodePtr node);
	Void _TranslateCondition(NodePtr node);
	Void _Translate0(NodePtr node);
	Void _Translate1(NodePtr node);
	Void _Translate2(NodePtr node);
	Void _ReadArguments(NodePtr node);
private:
	Vector<VarNode> nodeStack;
	NodeBuilderPtr builder;
	Vector<VarNode> argsArray;
	Uint32 methodHash;
};


LAME_END2

#endif // ~__LAME_SCRIPT__PERFORMER_PERFORMER__