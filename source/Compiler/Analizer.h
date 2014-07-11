#ifndef __LAME_COMPILER_ANALIZER__
#define __LAME_COMPILER_ANALIZER__

#include "Define.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

class LAME_API CodeAnalizer {
	typedef Script::NodePtr NodePtr;
private:
	typedef Vector<NodePtr> NodeList;
	typedef Vector<NodePtr>& NodeListRef;
	typedef const Vector<NodePtr>& NodeListRefC;
	typedef Vector<ObjectPtr> VarList;
	typedef Vector<ObjectPtr>& VarListRef;
	typedef const Vector<ObjectPtr>& VarListRefC;
public:
	Void Analize(NodeBuilderPtr nodeBuilder);
	Void Overload(Void);
private:
	Void _AnalizeList(NodeListRef nodeList);
	Void _Analize0(NodePtr n);
	Void _Analize1(NodePtr n);
	Void _Analize2(NodePtr n);
	Void _AnalizeCondition(NodePtr n);
	Void _AnalizeNew(NodePtr n);
	Void _AnalizeSelection(NodePtr n);
	Void _AnalizeCast(NodePtr n);
	Void _Read(NodePtr n, VariablePtr& left, VariablePtr& right);
private:
	Vector<VariablePtr> varStack;
	Vector<Buffer> nameStack;
	NodeBuilderPtr builder;
	Uint32 methodHash;
};

LAME_END2

#endif // ~__LAME_COMPILER_ANALIZER__