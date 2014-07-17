#ifndef __LAME_COMPILER_VIRTUAL_ANALIZER__
#define __LAME_COMPILER_VIRTUAL_ANALIZER__

#include "LowLevelStack.h"
#include "Register.h"
#include "ByteCodePrinter.h"

LAME_BEGIN2(Compiler)

class LAME_API VirtualAnalizer {
protected:
	typedef Script::NodePtr NodePtr;
	typedef Script::VariablePtr VariablePtr;
	typedef Script::ObjectPtr ObjectPtr;
	typedef Core::Buffer Buffer;
	typedef Core::BufferRefC BufferRefC;
	typedef Script::NodeBuilderPtr NodeBuilderPtr;
	typedef Script::ScopePtr ScopePtr;
	typedef Script::ClassPtr ClassPtr;
protected:
	typedef Core::Vector<NodePtr> NodeList;
	typedef Core::Vector<NodePtr>& NodeListRef;
	typedef Core::Vector<VariablePtr> VarList;
	typedef Core::Vector<VariablePtr>& VarListRef;
	typedef Core::Vector<Buffer> NameList;
	typedef Core::Vector<Buffer>& NameListRef;
protected:
	virtual Void AnalizeBinary(VariablePtr source, VariablePtr left, VariablePtr right) = 0;
	virtual Void AnalizeUnary(VariablePtr source, VariablePtr left) = 0;
	virtual Void AnalizeNew(VariablePtr source, VariablePtr left, ClassPtr right) = 0;
	virtual Void AnalizeSelection(VariablePtr source, VariablePtr left, VariablePtr right) = 0;
	virtual Void AnalizeCondition(NodePtr n, VariablePtr source) = 0;
	virtual Void AnalizeCast(VariablePtr source, VariablePtr left) = 0;
	virtual Void AnalizeInvoke(NodePtr n, VariablePtr source) = 0;
	virtual Void AnalizePush(VariablePtr var) = 0;
	virtual Void AnalizePop(VariablePtr& var) = 0;
protected:
	Void Treat(NodeListRef nodeList);
	Void Read(NodePtr node, VariablePtr& left, VariablePtr& right);
	Void Write(VariablePtr var);
protected:
	LowLevelStackPtr lowLevelStack;
	VarList varStack;
	NameList nameStack;
	NodeBuilderPtr nodeBuilder;
	Uint32 methodHash;
	ScopePtr rootScope;
	NodePtr currentNode;
	ByteCodePrinterPtr byteCodePrinter;
public:
	VirtualAnalizer() {
		this->byteCodePrinter =
			ByteCodePrinter::GetInstance();
	}
public:
	Void Analize(LowLevelStackPtr lowLevelStack,
		NodeBuilderPtr nodeBuilder, ScopePtr rootScope);
private:
	RegisterPtr _GetSourceRegister(VariablePtr variable);
private:
	Void _AnalizeBinary(NodePtr n);
	Void _AnalizeUnary(NodePtr n);
	Void _AnalizeNew(NodePtr n);
	Void _AnalizeSelection(NodePtr n);
	Void _AnalizeCondition(NodePtr n);
	Void _AnalizeCast(NodePtr n);
	Void _AnalizeInvoke(NodePtr n);
};

LAME_END2

#endif // ~__LAME_COMPILER_VIRTUAL_ANALIZER__