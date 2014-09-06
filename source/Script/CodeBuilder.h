#ifndef __LAME_SCRIPT_CODE_BUILDER__
#define __LAME_SCRIPT_CODE_BUILDER__

#include "Define.h"
#include "NodeBuilder.h"
#include "StackVar.h"
#include "Node.h"
#include "Exception.h"
#include "Class.h"
#include "Variable.h"
#include "Interface.h"
#include "Method.h"
#include "CodeNode.h"
#include "LittleCalculator.h"

LAME_BEGIN2(Script)

class LAME_API CodeBuilder {
public:
	typedef Core::Deque<NodePtr> NodeList;
	typedef Core::Deque<NodePtr>& NodeListRef;
public:
	CodeBuilder() :
		littleCalculator(this)
	{
	}
public:
	~CodeBuilder();
public:
	Void Run(
		SyntaxBuilderPtr nodeBuilder,
		ScopePtr         rootScope,
		SegmentPtr       codeSegment);
public:
	inline Vector<CodeNodePtr>& GetCodeList()   { return this->codeList;          }
	inline Vector<ObjectPtr>&   GetClassList()  { return this->classList;         }
	inline LittleCalculatorPtr  GetCalculator() { return &this->littleCalculator; }
private:
	Void _Read(NodePtr node, VariablePtr& left, VariablePtr& right);
	Void _Run(NodeListRef nodeList, Bool makeBackup = FALSE);
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
	Void _Cast(VariablePtr var, ObjectPtr type);
	Void _StrongCast(VariablePtr var, ClassPtr type);
	Void _Invoke(NodePtr n);
	Void _Return(NodePtr n);
	Void _Finish(NodePtr n);
	Void _Array(NodePtr n);
private:
	Void _Push(CodeNodePtr codeNode);
	CodeNodePtr _Pop(Void);
private:
	Vector<ObjectPtr> classList;
	Vector<ObjectPtr> methodList;
	StackVar variableStack;
	NodePtr currentNode;
	VariablePtr lastResult;
	Vector<CodeNodePtr> codeList;
	CodeNodePtr currentCode;
	NodePtr rememberedInvoke;
	Stack<CodeNodePtr> codeStack;
	CodeNodePtr currentMethod;
	LittleCalculator littleCalculator;
	Vector<NodePtr> nodeList;
	NodePtr lastNode;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CODE_BUILDER__