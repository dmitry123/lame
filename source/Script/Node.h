#ifndef __LAME_SCRIPT__BUILDER_NODE__
#define __LAME_SCRIPT__BUILDER_NODE__

#include "LexNode.h"

LAME_BEGIN2(Script)

typedef enum {
	kScriptFlagDefault = 0x0000,
	kScriptFlagImplemented = 0x0001,
	kScriptFlagFinal = 0x0002,
	kScriptFlagPublic = 0x004,
	kScriptFlagPrivate = 0x008,
	kScriptFlagProtected = 0x0010,
	kScriptFlagStatic = 0x0020,
	kScriptFlagNative = 0x0040,
	kScriptFlagAbstract = 0x0080,
	kScriptFlagArray = 0x0100,
	kScriptFlagArgumentList = 0x0200,
	kScriptFlagTemplate = 0x0400,
	kScriptFlagOverride = 0x0800,
	kScriptFlagDeprecated = 0x1000,
	kScriptFlagForEach = 0x2000,
	kScriptFlagEnum = 0x4000,
	kScriptFlagWhileHasDo = 0x8000,
	kScriptFlagInvocation = 0x10000,
	kScriptFlagSynchronized = 0x20000,
	kScriptFlagTransient = 0x40000,
	kScriptFlagVolatile = 0x8000,
	kScriptFlagAnnotation = 0x10000,
	kScriptFlagInitialize = 0x20000,
	kScriptFlagScope = 0x40000
} FlagID;

typedef enum {
	kScriptNodeUnknown,
	kScriptNodeDefault,
	kScriptNodeFunction,
	kScriptNodeVariable,
	kScriptNodeClass,
	kScriptNodeAnonymous,
	kScriptNodeInterface,
	kScriptNodeCondition,
	kScriptNodeEntry,
	kScriptNodeInvoke,
	kScriptNodeAlloc,
	kScriptNodeArray,
	kScriptNodeArgumentList,
	kScriptNodeStatic,
	kScriptNodeAnnotation
} NodeID;

class LAME_API Node {
	friend class SyntaxBuilder;
public:
	      Buffer word;
	const LexNodePtr lex;
	      NodeID id;
	const NodePtr parent;
	const NodePtr previous;
public:
	NodePtr typeNode;
	NodePtr templateNode;
	ObjectPtr var;
	Uint32 flags;
	Deque<NodePtr> argList;
	Deque<NodePtr> blockList;
	Deque<NodePtr> elseList;
	NodePtr elseNode;
	NodePtr next;
	Uint32 depth;
	NodePtr finalNode;
	Bool wasItBrace;
	Bool wasItLeft;
public:
	struct {
		NodePtr extendNode;
		Vector<NodePtr> implementNode;
		NodePtr superNode;
	} classInfo;
	struct {
		Deque<NodePtr> beginList;
		Deque<NodePtr> conditionList;
		Deque<NodePtr> nextList;
	} forInfo;
	struct {
		Deque<NodePtr> throwsList;
		Uint32 invokeHash;
	} methodInfo;
	struct {
		Deque<NodePtr> caseList;
		Bool hasBreak;
	} switchInfo;
public:
	Node(Buffer word, NodeID id, LexNodePtr lex,
		NodePtr parent, NodePtr prev);
public:
	Void ShuntingYard(Void);
	Void Extend(NodePtr node);
	Void Implement(NodePtr node);
	Void Template(NodePtr node);
	Void Type(NodePtr node);
public:
	inline SyntaxBuilderPtr GetBuilder() {
		return this->syntaxBuilder;
	}
private:
	SyntaxBuilderPtr syntaxBuilder;
};

LAME_END2

#endif // ~__LAME_SCRIPT__BUILDER_NODE__