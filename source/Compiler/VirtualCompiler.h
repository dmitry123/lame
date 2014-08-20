#ifndef __LAME_COMPILER_VIRTUAL_COMPILER__
#define __LAME_COMPILER_VIRTUAL_COMPILER__

#include "LowLevelStack.h"
#include "Register.h"
#include "ByteCodePrinter.h"
#include "VariableStack.h"

LAME_BEGIN2(Compiler)

class LAME_API VirtualCompiler {
protected:
	typedef Core::Deque<NodePtr> NodeList;
	typedef Core::Deque<NodePtr>& NodeListRef;
protected:
	virtual Void OnBinary(VariablePtr left, VariablePtr right) = 0;
    virtual Void OnUnary(VariablePtr var) = 0;
    virtual Void OnReturn(VariablePtr var) = 0;
    virtual Void OnLoad(VariablePtr var) = 0;
    virtual Void OnStore(VariablePtr var) = 0;
    virtual Void OnCast(VariablePtr source, VariablePtr type) = 0;
	virtual Void OnInvoke(MethodPtr method) = 0;
protected:
	Void Read(NodePtr node, VariablePtr& left, VariablePtr& right);
	Void Run(NodeListRef nodeList);
	Void Write(VariablePtr var);
	Void Print(StringC message, ...);
protected:
    VariableStack variableStack;
	NodeBuilderPtr nodeBuilder;
	Uint32 methodHash;
	ScopePtr rootScope;
	NodePtr currentNode;
	ByteCodePrinterPtr byteCodePrinter;
public:
	VirtualCompiler() {
		this->byteCodePrinter = ByteCodePrinter::GetInstance();
	}
protected:
	Void Analize(NodeBuilderPtr nodeBuilder, ScopePtr rootScope);
private:
	Void _ForEachClass(ScopePtr rootScope);
	Void _ForEachMethod(ScopePtr rootScope);
private:
	Void _AnalizeBinary(NodePtr n);
	Void _AnalizeUnary(NodePtr n);
	Void _AnalizeNew(NodePtr n);
	Void _AnalizeSelection(NodePtr n);
	Void _AnalizeCondition(NodePtr n);
	Void _AnalizeCast(NodePtr n);
	Void _AnalizeInvoke(NodePtr n);
	Void _AnalizeReturn(NodePtr n);
private:
    Vector<ObjectPtr> classList;
	Vector<ObjectPtr> methodList;
	Vector<Uint32> addressStack;
};

LAME_END2

#endif // ~__LAME_COMPILER_VIRTUAL_COMPILER__