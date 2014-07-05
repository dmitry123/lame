#ifndef __LAME_SCRIPT__PERFORMER_PERFORMER__
#define __LAME_SCRIPT__PERFORMER_PERFORMER__

#include "NodeBuilder.h"
#include "Object.h"

LAME_BEGIN2(Script)

class LAME_API NodePerformer {
public:
	typedef Void(*ForEachNode)(NodePtr node);
public:
	typedef       Vector<NodePtr>  NodeList;
	typedef       Vector<NodePtr>& NodeListRef;
	typedef const Vector<NodePtr>& NodeListRefC;
	typedef       Vector<ObjectPtr>  VarList;
	typedef       Vector<ObjectPtr>& VarListRef;
	typedef const Vector<ObjectPtr>& VarListRefC;
public:
	NodePerformer() :
		builder(0)
	{
	}
public:
	~NodePerformer() {
	}
public:
	Void Perform(NodeBuilderPtr builder);
	Bool Evaluate(NodeListRef nodeList);
public:
	static inline ObjectPtr FindClass(NodePtr node, BufferRefC name) {
		return _Find(Object::Type::Class, node, name);
	}
	static inline ObjectPtr FindVariable(NodePtr node, BufferRefC name) {
		return _Find(Object::Type::Variable, node, name);
	}
private:
	typedef struct {
		NodePtr node;
		ObjectPtr var;
	} VarNode, *VarNodePtr;
private:
	Void _EvaluateNew(NodePtr node);
	Void _EvaluateExpression0(NodePtr node);
	Void _EvaluateExpression1(NodePtr node);
	Void _EvaluateExpression2(NodePtr node);
	Void _ForEachNode(ForEachNode callback, NodeID id = kScriptNodeUnknown);
	Void _ReadArguments(NodePtr node);
private:
	static Void _ForEachClassPrototype(NodePtr n);
	static Void _ForEachInterfacePrototype(NodePtr n);
	static Void _ForEachClassDeclare(NodePtr n);
	static Void _ForEachInterfaceDeclare(NodePtr n);
	static Void _ForEachClassInherit(NodePtr n);
	static Void _ForEachInterfaceInherit(NodePtr n);
	static Void _ForEachConstDeclare(NodePtr n);
	static Void _ForEachVariableDeclare(NodePtr n);
private:
	static ObjectPtr _Find(Object::Type type, NodePtr node, BufferRefC name);
	static Void _DeclareConstant(NodePtr node);
	static Void _ForEachNode(NodePtr node, ForEachNode callback, NodeID id = kScriptNodeUnknown);
private:
	Vector<VarNode> nodeStack;
	NodeBuilderPtr builder;
	Vector<VarNode> argsArray;
	Uint32 methodHash;
};


LAME_END2

#endif // ~__LAME_SCRIPT__PERFORMER_PERFORMER__