#ifndef __LAME_SCRIPT__BUILDER_BUILDER__
#define __LAME_SCRIPT__BUILDER_BUILDER__

#include "FileParser.h"
#include "Node.h"

LAME_BEGIN2(Script)

class LAME_API NodeBuilder {
public:
	Void Build(FileParserPtr p);
public:
	typedef Vector<LexNodePtr>::iterator
		Iterator;
public:
	inline NodePtr GetRootNode() {
		return this->rootNode_;
	}
private:
	Iterator _BuildFunction(NodePtr& parent, Iterator i);
	Iterator _BuildVariable(NodePtr& parent, Iterator i);
	Iterator _BuildClass(NodePtr& parent, Iterator i);
	Iterator _BuildCondition(NodePtr& parent, Iterator i);
	Iterator _BuildEntry(NodePtr& parent, Iterator i);
	Iterator _BuildArguments(NodePtr& parent, Iterator i);
private:
	Uint32 _GetCountOfArguments(Iterator i);
private:
	Iterator _Build(NodePtr& node, Iterator i);
	Void _Reset(Void);
private:
	NodePtr _CreateNode(LexNodePtrC lex, NodeID id);
	NodePtr _RemoveNode(NodePtr node);
private:
	Void _Push(Vector<NodePtr>* stack);
	Vector<NodePtr>* _Pop(Void);
private:
	Vector<NodePtr> nodeList_;
	NodePtr parentNode_;
	NodePtr prevNode_;
	Vector<NodePtr>* nodeQueue_;
	List<Vector<NodePtr>*> stackNodeQueue_;
	NodePtr rootNode_;
	FileParserPtr parser_;
	Uint32 modificators_;
public:
	 NodeBuilder();
	~NodeBuilder();
};

LAME_END2

#endif // ~__LAME_SCRIPT__BUILDER_BUILDER__