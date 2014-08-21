#ifndef __LAME_SCRIPT__BUILDER_BUILDER__
#define __LAME_SCRIPT__BUILDER_BUILDER__

#include "FileParser.h"
#include "Node.h"

LAME_BEGIN2(Script)

typedef enum {
	kScriptLexSequenceDefault,
	kScriptLexSequenceArray,
	kScriptLexSequenceArguments,
	kScriptLexSequenceTemplate,
	kScriptLexSequenceCast,
	kScriptLexSequenceMethod,
	kScriptLexSequenceVariable,
	kScriptLexSequenceInvoke,
	kScriptLexSequenceAlloc,
	kScriptLexSequenceNew,
	kScriptLexSequenceEnum,
	kScriptLexSequenceTernary,
	kScriptLexSequenceAmount
} LexSequenceID;

class LAME_API NodeBuilder {
public:
	Void Build(FileParserPtr p);
public:
	typedef Deque<LexNodePtr>::iterator
		Iterator;
public:
	inline NodePtr GetRootNode() { return this->rootNode_; }
	inline Vector<NodePtr>& GetNodeList() { return this->nodeList_; }
private:
	Iterator _BuildFunction(NodePtr& parent, Iterator i);
	Iterator _BuildVariable(NodePtr& parent, Iterator i);
	Iterator _BuildClass(NodePtr& parent, Iterator i);
	Iterator _BuildCondition(NodePtr& parent, Iterator i);
	Iterator _BuildEntry(NodePtr& parent, Iterator i);
	Iterator _BuildArray(NodePtr& parent, Iterator i);
	Iterator _BuildArgumentList(NodePtr& parent, Iterator i);
	Iterator _BuildTemplate(NodePtr& parent, Iterator i);
	Iterator _BuildCatch(NodePtr& parent, Iterator i);
	Iterator _BuildNew(NodePtr& parent, Iterator i);
	Iterator _BuildEnum(NodePtr& parent, Iterator i);
	Iterator _BuildTernary(NodePtr& parent, Iterator i);
private:
	Uint32 _GetCountOfArguments(Iterator i);
	Iterator _SkipArguments(Iterator i);
	Iterator _Build(NodePtr& node, Iterator i);
	Void _Reset(Void);
	NodePtr _CreateNode(LexNodePtrC lex, NodeID id);
	NodePtr _RemoveNode(NodePtr node);
	Void _Push(Deque<NodePtr>* stack);
	Deque<NodePtr>* _Pop(Void);
	Bool _CheckLex(Iterator i, Vector<LexID> lexList);
	Void _DeclareSequence(LexSequenceID id, Vector<LexID> lexList);
	Bool _CheckSequence(Iterator i, LexSequenceID id);
	Void _ApplySemicolon(Void);
private:
	inline Bool _IsDefault(Iterator i)   { return this->_CheckSequence(i, kScriptLexSequenceDefault);   }
	inline Bool _IsArray(Iterator i)     { return this->_CheckSequence(i, kScriptLexSequenceArray);     }
	inline Bool _IsArguments(Iterator i) { return this->_CheckSequence(i, kScriptLexSequenceArguments); }
	inline Bool _IsTemplate(Iterator i)  { return this->_CheckSequence(i, kScriptLexSequenceTemplate);  }
	inline Bool _IsCast(Iterator i)      { return this->_CheckSequence(i, kScriptLexSequenceCast);      }
	inline Bool _IsMethod(Iterator i)    { return this->_CheckSequence(i, kScriptLexSequenceMethod);    }
	inline Bool _IsVariable(Iterator i)  { return this->_CheckSequence(i, kScriptLexSequenceVariable);  }
	inline Bool _IsInvoke(Iterator i)    { return this->_CheckSequence(i, kScriptLexSequenceInvoke);    }
	inline Bool _IsAlloc(Iterator i)     { return this->_CheckSequence(i, kScriptLexSequenceAlloc);     }
	inline Bool _IsNew(Iterator i)       { return this->_CheckSequence(i, kScriptLexSequenceNew);       }
	inline Bool _IsEnum(Iterator i)      { return this->_CheckSequence(i, kScriptLexSequenceEnum);      }
	inline Bool _IsTernary(Iterator i)   { return this->_CheckSequence(i, kScriptLexSequenceTernary);   }
private:
	Vector<NodePtr> nodeList_;
	NodePtr parentNode_;
	NodePtr prevNode_;
	Deque<NodePtr>* nodeQueue_;
	List<Deque<NodePtr>*> stackNodeQueue_;
	NodePtr rootNode_;
	FileParserPtr parser_;
	Uint32 modificators_;
	Vector<Vector<LexID>> sequenceList_;
	Vector<LexNodePtr> incStack_;
	Vector<LexNodePtr> decStack_;
public:
	 NodeBuilder();
	~NodeBuilder();
};

LAME_END2

#endif // ~__LAME_SCRIPT__BUILDER_BUILDER__