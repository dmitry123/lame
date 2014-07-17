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
	Void Build(NodeBuilderPtr nodeBuilder, ScopePtr rootScope);
private:
	Void _ForEachNodeTrace(NodePtr n);
	Void _ForEachModificatorSet(NodePtr n);
	Void _ForEachNodeFind(NodePtr n);
	Void _ForEachClassPrototype(NodePtr n);
	Void _ForEachInterfacePrototype(NodePtr n);
	Void _ForEachClassDeclare(NodePtr n);
	Void _ForEachInterfaceDeclare(NodePtr n);
	Void _ForEachClassInherit(NodePtr n);
	Void _ForEachInterfaceInherit(NodePtr n);
	Void _ForEachConstDeclare(NodePtr n);
	Void _ForEachMethodDeclare(NodePtr n);
	Void _ForEachMethodRegister(NodePtr n);
	Void _ForEachVariableDeclare(NodePtr n);
private:
	Void _ForEachNode(NodePtr node, ScopePtr scope, ForEachNode callback, NodeID id);
	Void _Push(ScopePtr scope);
	ScopePtr _Pop(Void);
private:
	ScopePtr scope;
	Vector<ScopePtr> scopeStack;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SCOPE_BUILDER__