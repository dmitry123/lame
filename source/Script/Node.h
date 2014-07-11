#ifndef __LAME_SCRIPT__BUILDER_NODE__
#define __LAME_SCRIPT__BUILDER_NODE__

#include "LexNode.h"

LAME_BEGIN2(Script)

typedef enum {
	kScriptFlagDefault = 0x0000,
	kScriptFlagTemp = 0x0001,
	kScriptFlagType = 0x0002,
	kScriptFlagFinal = 0x0004,
	kScriptFlagFunction = 0x0008,
	kScriptFlagPublic = 0x0010,
	kScriptFlagPrivate = 0x0020,
	kScriptFlagProtected = 0x0040,
	kScriptFlagStatic = 0x0080,
	kScriptFlagNative = 0x0100,
	kScriptFlagAbstract = 0x0200
} FlagID;

typedef enum {
	kScriptNodeUnknown,
	kScriptNodeDefault,
	kScriptNodeFunction,
	kScriptNodeVariable,
	kScriptNodeClass,
	kScriptNodeInterface,
	kScriptNodeCondition,
	kScriptNodeEntry,
	kScriptNodeInvoke,
	kScriptNodeAlloc
} NodeID;

class LAME_API Node {
public:
	const Buffer word;
	const LexNodePtr lex;
	const NodeID id;
	const NodePtr parent;
	const NodePtr previous;
public:
	Buffer typeName;
	ObjectPtr var;
	Uint32 modificators;
	Buffer extends;
	Vector<Buffer> implements;
	Bool isMethodImpl;
	Bool isArray;
	VariablePtr thisVar;
	Buffer templates;
	Buffer templatesNew;
public:
	Vector<NodePtr> argList;
	Vector<NodePtr> blockList;
public:
	Node(
		Buffer word,
		NodeID id,
		LexNodePtr lex,
		NodePtr parent,
		NodePtr prev);
	~Node();
public:
	ScopeControllerPtr GetScope(Void);
public:
	inline Void MakeInvoke() {
		*((NodeID*)&this->id) = kScriptNodeInvoke;
	}
	inline Void MakeAlloc() {
		*((NodeID*)&this->id) = kScriptNodeAlloc;
	}
	inline Void MakeDefault() {
		*((NodeID*)&this->id) = kScriptNodeDefault;
	}
public:
	Void ShuntingYard(Void);
};

LAME_END2

#endif // ~__LAME_SCRIPT__BUILDER_NODE__