#ifndef __LAME_COMPILER_VIRTUAL_COMPILER__
#define __LAME_COMPILER_VIRTUAL_COMPILER__

#include "ByteCode.h"
#include "VariableStack.h"

LAME_BEGIN2(Compiler)

class LAME_API VirtualCompiler {
protected: /* Core Typedefs */
	typedef Core::Deque<NodePtr> NodeList;
	typedef Core::Deque<NodePtr>& NodeListRef;
protected: /* Script Typedefs */
	typedef Script::VariablePtr VariablePtr;
protected: /* Strict Virtual Methods */
	virtual Void OnBinary(VariablePtr left, VariablePtr right) = 0;
    virtual Void OnUnary(VariablePtr var) = 0;
	virtual Void OnTernary(NodePtr node, Bool state) = 0;
    virtual Void OnReturn(ClassPtr var) = 0;
    virtual Void OnLoad(VariablePtr var) = 0;
    virtual Void OnStore(VariablePtr var) = 0;
	virtual Void OnCast(VariablePtr source, ClassPtr type) = 0;
	virtual Void OnInvoke(MethodPtr method) = 0;
protected:
	Void _Read(NodePtr node, VariablePtr& left, VariablePtr& right);
	Void _Run(NodeListRef nodeList);
	Void _Write(VariablePtr var);
	Void _Print(StringC message, ...);
	Void _Cast(VariablePtr var, ObjectPtr type);
public:
	inline NodePtr GetNode() { return this->currentNode; }
	inline ScopePtr GetRoot() { return this->rootScope; }
	inline VariableStackPtr GetStack() { return &this->variableStack; }
	inline ByteCodePtr GetByteCode() { return this->byteCode; }
protected:
    VariableStack variableStack;
	NodeBuilderPtr nodeBuilder;
	Uint32 methodHash;
	ScopePtr rootScope;
	NodePtr currentNode;
	ByteCodePtr byteCode;
public:
	VirtualCompiler() {
		this->byteCode = NULL;
	}
public:
	virtual ~VirtualCompiler() {
		delete this->byteCode;
	}
protected:
	Void Run(
		NodeBuilderPtr nodeBuilder,
		ScopePtr       rootScope,
		SegmentPtr     codeSegment);
private:
	Void _ForEachClass(ScopePtr rootScope);
	Void _ForEachMethod(ScopePtr rootScope);
private:
	Void _Binary(NodePtr n);
	Void _Unary(NodePtr n);
	Void _Ternary(NodePtr n);
	Void _New(NodePtr n);
	Void _Selection(NodePtr n);
	Void _Condition(NodePtr n);
	Void _Cast(NodePtr n);
	Void _StrongCast(VariablePtr var, ClassPtr type);
	Void _Invoke(NodePtr n);
	Void _Return(NodePtr n);
	Void _Finish(NodePtr n);
private:
	Void _Push(SegmentPtr segment);
	SegmentPtr _Pop(Void);
private:
    Vector<ObjectPtr> classList;
	Vector<ObjectPtr> methodList;
	Vector<Uint32> addressStack;
	Stack<SegmentPtr> segmentStack;
	Vector<SegmentPtr> segmentList;
	Uint32 segmentOffset;
};

LAME_END2

#endif // ~__LAME_COMPILER_VIRTUAL_COMPILER__