#ifndef __LAME_COMPILER_CODE_TRANSLATOR__
#define __LAME_COMPILER_CODE_TRANSLATOR__

#include "VirtualAnalizer.h"

LAME_BEGIN2(Compiler)

using namespace Core;
using namespace Script;

class LAME_API CodeTranslator : public VirtualAnalizer {
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
private:
	Void _New(Uint32 command);
	Void _Read1(VariablePtr& left);
	Void _Read2(VariablePtr& left, VariablePtr& right);
	Void _Write1(VariablePtr var);
	Void _Jump(Uint32 address);
	Void _Const(Uint32 value);
	Void _Return(VariablePtr var);
	Void _Mov(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right);
	Void _Math(Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right);
	Void _Bool(Uint32 cmp, Uint32 command, VariablePtr source, VariablePtr left, VariablePtr right);
	Void _Unary(Uint32 command, VariablePtr source);
};

LAME_END2

#endif // ~__LAME_COMPILER_CODE_TRANSLATOR__