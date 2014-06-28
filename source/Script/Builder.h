#ifndef __LAME_SCRIPT__BUILDER__
#define __LAME_SCRIPT__BUILDER__

#include "Define.h"
#include "Exception.h"
#include "Parser.h"

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
	kScriptFlagNative = 0x0100
} ScriptFlagID;

typedef enum {
	kScriptNodeDefault,
	kScriptNodeFunction,
	kScriptNodeVariable,
	kScriptNodeClass,
	kScriptNodeCondition,
	kScriptNodeEntry,
	kScriptNodeInvoke
} ScriptNodeID;

class LAME_API ScriptNode {
public:
	const Buffer word;
	const ScriptLexNodePtr lex;
	const ScriptNodeID id;
	const ScriptNodePtr parent;
	const ScriptNodePtr prev;
public:
	Buffer typeName;
	ScriptVarPtr var;
    ScriptVarPtr returnVar;
	Uint32 args;
	Uint32 modificators;
	Bool result;
public:
	Vector<ScriptNodePtr> childList;
	Vector<ScriptNodePtr> argList;
	Vector<ScriptNodePtr> blockList;
public:
	ScriptNode(Buffer word, ScriptNodeID id, ScriptLexNodePtr lex, ScriptNodePtr parent, ScriptNodePtr prev) :
		word(word),
		id(id),
		lex(lex),
		parent(parent),
		prev(prev)
	{
        this->returnVar = 0;
		this->var = 0;
		this->args = 0;
		this->modificators = 0;
		this->result = 0;
	}
	~ScriptNode();
public:
	Void Order(Void);
	ScriptNodePtr FindChild(BufferRefC name);
	ScriptNodePtr FindBlock(BufferRefC name);
public:
	Void MakeInvoke() { *((ScriptNodeID*)&this->id) = kScriptNodeInvoke; }
	Void MakeDefault() { *((ScriptNodeID*)&this->id) = kScriptNodeDefault; }
private:
	Void _Order(Vector<ScriptNodePtr>* list);
};

class LAME_API ScriptBuilder {
	friend class ScriptPerformer;
public:
	typedef Vector<ScriptLexNodePtr>::iterator Iterator;
public:
	Void Build(ScriptParserPtr parser);
private:
	Iterator _BuildFunction(ScriptNodePtr& parent, Iterator i);
	Iterator _BuildVariable(ScriptNodePtr& parent, Iterator i);
	Iterator _BuildClass(ScriptNodePtr& parent, Iterator i);
	Iterator _BuildCondition(ScriptNodePtr& parent, Iterator i);
	Iterator _BuildEntry(ScriptNodePtr& parent, Iterator i);
	Iterator _BuildArguments(ScriptNodePtr& parent, Iterator i);
private:
	Iterator _Build(ScriptNodePtr& node, Iterator i);
	Void _Reset(Void);
private:
	ScriptNodePtr _CreateNode(ScriptLexNodePtrC lex, ScriptNodeID id);
	ScriptNodePtr _RemoveNode(ScriptNodePtr node);
private:
	Void _Push(Vector<ScriptNodePtr>* stack);
	Vector<ScriptNodePtr>* _Pop(Void);
private:
	Vector<ScriptNodePtr> nodeList_;
	ScriptNodePtr parentNode_;
	ScriptNodePtr prevNode_;
	Vector<ScriptNodePtr>* nodeQueue_;
	List<Vector<ScriptNodePtr>*> stackNodeQueue_;
	ScriptNodePtr rootNode_;
	ScriptParserPtr parser_;
public:
	 ScriptBuilder();
	~ScriptBuilder();
};

LAME_END2

#endif // ~__LAME_SCRIPT__BUILDER__