#ifndef __LAME_SCRIPT_CODE_BUILDER__
#define __LAME_SCRIPT_CODE_BUILDER__

#include "NodeBuilder.h"
#include "Variable.h"
#include "Interface.h"
#include "Class.h"
#include "LittleCalculator.h"
#include "NodeWalker.h"

LAME_BEGIN2(Script)

class LAME_API CodeBuilder :
	public NodeWalker::Listener,
	public NodeWalker::Optimizer
{
public:
	typedef NodeWalker::NodeList NodeList;
	typedef NodeWalker::Iterator Iterator;
public:
	CodeBuilder() : littleCalculator(this),
		Listener(&nodeWalker),
		Optimizer(&nodeWalker)
	{
	}
public:
	~CodeBuilder();
public:
	Void Build(
		SyntaxBuilderPtr nodeBuilder,
		ScopePtr         rootScope);
private:
	Void onBinary(NodePtr n) override;
	Void onUnary(NodePtr n) override;
	Void onTernary(NodePtr n) override;
	Void onCondition(NodePtr n) override;
	Void onInvoke(NodePtr n) override;
	Void onNew(NodePtr n) override;
	Void onReturn(NodePtr n) override;
	Void onIndex(NodePtr n) override;
private:
	inline Iterator onBegin(Void) override
		{ return this->nodeList.begin(); }
	inline Iterator onEnd(Void) override
		{ return this->nodeList.end(); }
	inline Uint32 onSize(Void) override
		{ return Uint32(this->nodeList.size()); }
	inline Iterator onInsert(Iterator i, NodePtr n) override
		{ return this->nodeList.insert(i, n); }
	inline Iterator onErase(Iterator i) override
		{ return this->nodeList.erase(i); }
	inline Void onPush(NodePtr n) override
		{ this->nodeList.push_back(n); }
	inline Void onPop(Void) override
		{ this->nodeList.pop_back(); }
public:
	inline LittleCalculatorPtr GetCalculator() {
		return &this->littleCalculator;
	}
private:
	Void _Cast(VariablePtr var, ObjectPtr type);
	Void _StrongCast(VariablePtr var, ClassPtr type);
	Void _Save(NodePtr n);
private:
	LittleCalculator littleCalculator;
	Vector<NodePtr> nodeList;
	NodeWalker nodeWalker;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CODE_BUILDER__