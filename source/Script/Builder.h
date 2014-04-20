#ifndef __LAME_SCRIPT__BUILDER__
#define __LAME_SCRIPT__BUILDER__

#include "Define.h"
#include "Exception.h"
#include "Parser.h"

LAME_BEGIN

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
	const ScriptLexPtr lex;
	const ScriptNodeID id;
	const ScriptNodePtr parent;
public:
	Buffer typeName;
	Vector<ScriptNodePtr> childList;
	Vector<ScriptNodePtr> argList;
	ScriptVarPtr var;
	Uint32 args;
public:
	ScriptNode(Buffer word, ScriptNodeID id, ScriptLexPtr lex, ScriptNodePtr parent) :
		word(word),
		id(id),
		lex(lex),
		parent(parent)
	{
		this->var = 0;
		this->args = 0;
	}
public:
	Void Order(Void);
	ScriptNodePtr FindChild(BufferRefC name);
public:
	Void MakeInvoke() { *((ScriptNodeID*)&this->id) = kScriptNodeInvoke; }
	Void MakeDefault() { *((ScriptNodeID*)&this->id) = kScriptNodeDefault; }
private:
	Void _Order(Vector<ScriptNodePtr>* list);
};

class LAME_API ScriptBuilder {
	friend ScriptPerformer;
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
	Vector<ScriptNodePtr>* nodeQueue_;
	List<Vector<ScriptNodePtr>*> stackNodeQueue_;
	ScriptNodePtr rootNode_;
	ScriptParserPtr parser_;
public:
	ScriptBuilder();
	~ScriptBuilder();
};

LAME_END

#endif // ~__LAME_SCRIPT__BUILDER__