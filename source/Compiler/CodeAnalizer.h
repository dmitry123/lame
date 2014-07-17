#ifndef __LAME_COMPILER_CODE_ANALIZER__
#define __LAME_COMPILER_CODE_ANALIZER__

#include "VirtualAnalizer.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

class LAME_API CodeAnalizer : public VirtualAnalizer {
	typedef Script::NodePtr NodePtr;
private:
	typedef Vector<NodePtr> NodeList;
	typedef Vector<NodePtr>& NodeListRef;
	typedef const Vector<NodePtr>& NodeListRefC;
	typedef Vector<ObjectPtr> VarList;
	typedef Vector<ObjectPtr>& VarListRef;
	typedef const Vector<ObjectPtr>& VarListRefC;
private:
	Void AnalizeBinary(VariablePtr source, VariablePtr left, VariablePtr right) override;
	Void AnalizeUnary(VariablePtr source, VariablePtr left) override;
	Void AnalizeNew(VariablePtr source, VariablePtr left, ClassPtr right) override;
	Void AnalizeSelection(VariablePtr source, VariablePtr left, VariablePtr right) override;
	Void AnalizeCondition(NodePtr n, VariablePtr source) override;
	Void AnalizeCast(VariablePtr source, VariablePtr left) override;
	Void AnalizeInvoke(NodePtr n, VariablePtr source) override;
	Void AnalizePush(VariablePtr var) override;
	Void AnalizePop(VariablePtr& var) override;
};

LAME_END2

#endif // ~__LAME_COMPILER_CODE_ANALIZER__