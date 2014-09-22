#ifndef __LAME_SCRIPT_NODE_WALKER__
#define __LAME_SCRIPT_NODE_WALKER__

#include "Variable.h"
#include "Node.h"
#include "StackVar.h"
#include "Method.h"

LAME_BEGIN2(Script)

class LAME_API NodeWalker {
public:
	typedef Deque<NodePtr>& NodeList;
	typedef Vector<NodePtr>::iterator Iterator;
public:
	typedef class Listener {
	public:
		virtual Void onBinary(NodePtr n) {}
		virtual Void onUnary(NodePtr n) {}
		virtual Void onTernary(NodePtr n) {}
		virtual Void onCondition(NodePtr n) {}
		virtual Void onInvoke(NodePtr n) {}
		virtual Void onNew(NodePtr n) {}
		virtual Void onReturn(NodePtr n) {}
		virtual Void onIndex(NodePtr n) {}
	public:
		Listener(NodeWalkerPtr nodeWalker) :
			nodeWalker(nodeWalker)
		{
		}
	public:
		inline NodeWalkerPtr GetWalker() {
			return this->nodeWalker;
		}
	private:
		NodeWalkerPtr nodeWalker;
	} *ListenerPtr;
public:
	typedef class Optimizer {
	public:
		virtual Iterator onBegin(Void) = 0;
		virtual Iterator onEnd(Void) = 0;
		virtual Uint32   onSize(Void) = 0;
		virtual Iterator onInsert(Iterator i, NodePtr n) = 0;
		virtual Iterator onErase(Iterator i) = 0;
		virtual Void     onPush(NodePtr n) = 0;
		virtual Void     onPop(Void) = 0;
	public:
		Optimizer(NodeWalkerPtr nodeWalker) :
			nodeWalker(nodeWalker)
		{
		}
	public:
		inline NodeWalkerPtr GetWalker2() {
			return this->nodeWalker;
		}
	private:
		NodeWalkerPtr nodeWalker;
	} *OptimizerPtr;
public:
	Void Walk(ListenerPtr nodeListener, OptimizerPtr nodeOptimizer, NodePtr rootNode,
		ScopePtr rootScope, VoidP userData = NULL);
public:
	Void Run(NodeList nodeList, Bool backupStack = FALSE);
	Void Push(ObjectPtr object, Bool makeCallback = TRUE);
	ObjectPtr Pop(Void);
	Void Test(NodePtr node);
	ObjectPtr TestArgument(ClassPtr argType);
public:
	inline ListenerPtr    GetListener()  { return this->nodeListener;  }
	inline NodePtr        GetRootNode()  { return this->rootNode;      }
	inline StackVarPtr    GetStack()     { return &this->stackVar;     }
	inline Set<MethodPtr> GetMethodSet() { return this->methodSet;     }
	inline NodePtr        GetNode()      { return this->currentNode;   }
	inline ObjectPtr      GetLeft()      { return this->leftVar;       }
	inline ObjectPtr      GetRight()     { return this->rightVar;      }
	inline ScopePtr       GetRootScope() { return this->rootScope;     }
	inline ObjectPtr      GetResult()    { return this->lastResult;    }
	inline NodePtr        GetLastNode()  { return this->lastNode;      }
	inline MethodPtr      GetMethod()    { return this->currentMethod; }
	inline ObjectPtr      GetSelection() { return this->lastSelection; }
public:
	template <class T> T* GetUserData() {
		return (T*)this->userData;
	}
private:
	Void _FindMethod(ScopePtr scope);
	Void _Cleanup(Void);
	Void _Selection(NodePtr node);
	Void _Invoke(NodePtr node);
private:
	ListenerPtr nodeListener;
	OptimizerPtr nodeOptimizer;
	NodePtr rootNode;
	StackVar stackVar;
	VoidP userData;
	Set<MethodPtr> methodSet;
	NodePtr currentNode;
	ObjectPtr leftVar;
	ObjectPtr rightVar;
	ScopePtr rootScope;
	ObjectPtr lastResult;
	NodePtr lastNode;
	NodePtr rememberedInvoke;
	MethodPtr currentMethod;
	ObjectPtr lastSelection;
};

LAME_END2

#endif // ~__LAME_SCRIPT_NODE_WALKER__