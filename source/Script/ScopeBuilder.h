#ifndef __LAME_SCRIPT_SCOPE_BUILDER__
#define __LAME_SCRIPT_SCOPE_BUILDER__

#include "ScopeController.h"
#include "NodeBuilder.h"

LAME_BEGIN2(Script)

class LAME_API ScopeBuilder {
	friend class CodeAnalizer;
private:
	typedef Void(*ForEachNode)(NodePtr node);
public:
	Void Build(NodeBuilderPtr nodeBuilder, ScopeControllerPtr scopeController);
public:
	static Void _ForEachClassPrototype(NodePtr n);
	static Void _ForEachInterfacePrototype(NodePtr n);
	static Void _ForEachClassDeclare(NodePtr n);
	static Void _ForEachInterfaceDeclare(NodePtr n);
	static Void _ForEachClassInherit(NodePtr n);
	static Void _ForEachInterfaceInherit(NodePtr n);
	static Void _ForEachConstDeclare(NodePtr n);
	static Void _ForEachVariableDeclare(NodePtr n);
	static Void _ForEachVariableRegister(NodePtr n);
public:
	static ObjectPtr _FindClass(NodePtr node, BufferRefC name);
	static ObjectPtr _FindVariable(NodePtr node, BufferRefC name);
	static ObjectPtr _Find(Object::Type type, NodePtr node, BufferRefC name);
	static Void _DeclareConstant(NodePtr node);
	static Void _ForEachNode(NodePtr node, ForEachNode callback, NodeID id);
};

LAME_END2

#endif // ~__LAME_SCRIPT_SCOPE_BUILDER__