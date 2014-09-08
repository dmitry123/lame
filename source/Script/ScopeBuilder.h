#ifndef __LAME_SCRIPT_SCOPE_BUILDER__
#define __LAME_SCRIPT_SCOPE_BUILDER__

#include "NodeBuilder.h"
#include "Scope.h"

LAME_BEGIN2(Script)

class LAME_API ScopeBuilder {
private:
	typedef Void(ScopeBuilder::*ForEachNodeCallback)(NodePtr node);
	struct ForEachNode {
		ForEachNode(ForEachNodeCallback callback, ScopeBuilderPtr self) :
			callback(callback), self(*self)
		{
		}
		inline Void operator () (NodePtr node) {
			(this->self.*(this->callback))(node);
		}
		ForEachNodeCallback callback;
		ScopeBuilder& self;
	};
public:
	ScopeBuilder();
	~ScopeBuilder();
public:
	Void Build(NodePtr rootNode, ScopePtr rootScope);
private:
	Void _ForEachClassStatic(NodePtr n);
	Void _ForEachNodeTrace(NodePtr n);
	Void _ForEachModificatorSet(NodePtr n);
	Void _ForEachNodeFind(NodePtr n);
	Void _ForEachClassPrototype(NodePtr n);
	Void _ForEachInterfacePrototype(NodePtr n);
	Void _ForEachClassDeclare(NodePtr n);
	Void _ForEachClassVariableDeclare(NodePtr n);
	Void _ForEachInterfaceDeclare(NodePtr n);
	Void _ForEachClassInherit(NodePtr n);
	Void _ForEachInterfaceInherit(NodePtr n);
	Void _ForEachConstDeclare(NodePtr n);
	Void _ForEachMethodDeclare(NodePtr n);
	Void _ForEachMethodRegister(NodePtr n);
	Void _ForEachVariableDeclare(NodePtr n);
	Void _ForEachConstruction(NodePtr n);
    Void _ForEachCheckInheritance(NodePtr n);
    Void _ForEachNodeFlush(NodePtr n);
	Void _ForEachClassInit(NodePtr n);
private:
	Void _ForEachNode(NodePtr node, ScopePtr scope, ForEachNode callback, NodeID id);
	Void _Push(ScopePtr scope);
	ScopePtr _Pop(Void);
	Void _BuildClassList(NodePtr node);
	Buffer _GetNodePath(NodePtr node);
private:
	ScopePtr scope;
	Vector<ScopePtr> scopeStack;
	Vector<ClassPtr> lastStack;
	ClassPtr lastVar;
	Vector<NodePtr> classList;
	Uint32 depthLevel;
	ClassPtr rootConstruction;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SCOPE_BUILDER__