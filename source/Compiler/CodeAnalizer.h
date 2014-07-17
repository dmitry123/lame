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
	Void OverloadInteger(ScopePtr scope) override;
	Void OverloadFloat(ScopePtr scope) override;
	Void OverloadString(ScopePtr scope) override;
	Void OverloadObject(ScopePtr scope) override;
private:
	RegisterPtr Integer2Float(VariablePtr value) override;
	RegisterPtr Float2Integer(VariablePtr value) override;
	RegisterPtr Object2String(VariablePtr value) override;
	RegisterPtr String2Object(VariablePtr value) override;
	RegisterPtr Integer2String(VariablePtr value) override;
	RegisterPtr String2Integer(VariablePtr value) override;
	RegisterPtr Float2String(VariablePtr value) override;
	RegisterPtr String2Float(VariablePtr value) override;
private:
	Void AnalizeBinary(VariablePtr source, VariablePtr left, VariablePtr right) override;
	Void AnalizeUnary(VariablePtr source) override;
	Void AnalizeNew(VariablePtr source, ClassPtr left) override;
	Void AnalizeSelection(VariablePtr source, VariablePtr left, VariablePtr right) override;
	Void AnalizeCondition(NodePtr n, VariablePtr source) override;
	Void AnalizeCast(VariablePtr source, ClassPtr left) override;
	Void AnalizeInvoke(NodePtr n, VariablePtr source) override;
};

LAME_END2

#endif // ~__LAME_COMPILER_CODE_ANALIZER__