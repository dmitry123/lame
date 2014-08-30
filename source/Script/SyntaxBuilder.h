#ifndef __LAME_SCRIPT_SYNTAX_BUILDER__
#define __LAME_SCRIPT_SYNTAX_BUILDER__

#include "FileParser.h"
#include "Node.h"
#include "SequenceMatcher.h"
#include "Exception.h"

LAME_BEGIN2(Script)

class LAME_API SyntaxBuilder {
	friend class NodeBuilder;
private:
	enum {
		kScriptLexSequenceArguments,
		kScriptLexSequenceArray,
		kScriptLexSequenceTemplate,
		kScriptLexSequenceCast,
		kScriptLexSequenceMethod,
		kScriptLexSequenceVariable,
		kScriptLexSequenceInvoke,
		kScriptLexSequenceAlloc,
		kScriptLexSequenceNew
	};
public:
	typedef Deque<LexNodePtr>::iterator
		Iterator;
	typedef Deque<NodePtr> NodeList;
public:
	SyntaxBuilder(Void);
	~SyntaxBuilder(Void);
public:
	Void Build(FileParserPtr fileParser);
public:
	virtual Iterator If(NodePtr& node, Iterator i);
	virtual Iterator Else(NodePtr& node, Iterator i);
	virtual Iterator While(NodePtr& node, Iterator i);
	virtual Iterator Do(NodePtr& node, Iterator i);
	virtual Iterator For(NodePtr& node, Iterator i);
	virtual Iterator Try(NodePtr& node, Iterator i);
	virtual Iterator Catch(NodePtr& node, Iterator i);
	virtual Iterator Finally(NodePtr& node, Iterator i);
	virtual Iterator Variable(NodePtr& node, Iterator i);
	virtual Iterator Method(NodePtr& node, Iterator i);
	virtual Iterator Class(NodePtr& node, Iterator i);
	virtual Iterator New(NodePtr& node, Iterator i);
	virtual Iterator Arguments(NodePtr& node, Iterator i);
	virtual Iterator Template(NodePtr& node, Iterator i);
	virtual Iterator Entry(NodePtr& node, Iterator i);
	virtual Iterator Array(NodePtr& node, Iterator i);
	virtual Iterator Enum(NodePtr& node, Iterator i);
	virtual Iterator Ternary(NodePtr& node, Iterator i);
public:
	inline NodePtr GetRootNode() {
		return this->rootNode;
	}
private:
	Bool _ShallAvoidBrace(NodeList& list, Iterator& i);
	Bool _WasItBrace(NodePtr node);
	NodePtr _Create(Iterator& i, NodeID nodeType = kScriptNodeDefault);
	NodePtr _Remove(NodePtr node);
	NodePtr _Append(Iterator& i);
	Iterator _Append(Deque<NodePtr>* list, Iterator i);
	Void _Order(NodePtr node);
	Bool _Match(Uint32 id, Iterator i);
	Void _Reset(Void);
	Void _AllowModificators(NodePtr node, Uint32 modificators);
	Void _Combine(NodePtr node);
private:
	inline Deque<LexNodePtr>& _List() {
		return this->fileParser->GetLexList();
	}
	inline Iterator _End() {
		return this->fileParser->GetLexList().end();
	}
private:
	inline Bool _IsArray(Iterator i)     { return this->sequenceMatcher.Match(kScriptLexSequenceArray,     i, this->_End()); }
	inline Bool _IsArguments(Iterator i) { return this->sequenceMatcher.Match(kScriptLexSequenceArguments, i, this->_End()); }
	inline Bool _IsTemplate(Iterator i)  { return this->sequenceMatcher.Match(kScriptLexSequenceTemplate,  i, this->_End()); }
	inline Bool _IsCast(Iterator i)      { return this->sequenceMatcher.Match(kScriptLexSequenceCast,      i, this->_End()); }
	inline Bool _IsMethod(Iterator i)    { return this->sequenceMatcher.Match(kScriptLexSequenceMethod,    i, this->_End()); }
	inline Bool _IsVariable(Iterator i)  { return this->sequenceMatcher.Match(kScriptLexSequenceVariable,  i, this->_End()); }
	inline Bool _IsInvoke(Iterator i)    { return this->sequenceMatcher.Match(kScriptLexSequenceInvoke,    i, this->_End()); }
	inline Bool _IsAlloc(Iterator i)     { return this->sequenceMatcher.Match(kScriptLexSequenceAlloc,     i, this->_End()); }
	inline Bool _IsNew(Iterator i)       { return this->sequenceMatcher.Match(kScriptLexSequenceNew,       i, this->_End()); }
private:
	SequenceMatcher sequenceMatcher;
	Uint32 modificatorFlags;
	FileParserPtr fileParser;
	Vector<NodePtr> nodeContainer;
	NodePtr parentNode;
	NodePtr previousNode;
	NodePtr rootNode;
	Bool wasLastParenthesis;
};

LAME_END2

#endif // ~__LAME_SCRIPT_SYNTAX_BUILDER__