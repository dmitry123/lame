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

LAME_BEGIN2(Script)

class LAME_API CodeBuilder {
	typedef CodeNode::Code Code;
public:
	typedef Core::Deque<NodePtr> NodeList;
	typedef Core::Deque<NodePtr>& NodeListRef;
public:
	~CodeBuilder();
public:
	Void Run(
		SyntaxBuilderPtr nodeBuilder,
		ScopePtr         rootScope,
		SegmentPtr       codeSegment);
public:
	inline Vector<CodeNodePtr>& GetCodeList()   { return this->codeList;       }
	inline List<CodeMethod>&    GetMethodList() { return this->codeMethodList; }
	inline Vector<ObjectPtr>&   GetClassList()  { return this->classList;      }
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
	CodeNodePtr _Save(Code code,
		ObjectPtr left  = NULL,
		ObjectPtr right = NULL);
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
	List<CodeMethod> codeMethodList;
	CodeMethodPtr currentMethod;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CODE_BUILDER__