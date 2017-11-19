#ifndef __LAME_LANG__SCOPE_BUILDER__
#define __LAME_LANG__SCOPE_BUILDER__

#include "Foundation.h"
#include "Object.h"
#include "SyntaxError.h"
#include "TypeManager.h"
#include "ShuntingYard.h"

LAME_LANG_BEGIN

typedef class ScopeBuilder {
	typedef Logic(*Filter)(NodePtr);
public:
	ScopeBuilder(NodePtr root) :
		root(new Object(root)), parent(this->root)
	{
	}
	virtual ~ScopeBuilder() {
		delete this->root;
	}
public:
	inline void build() {
		this->build(this->root->getNode()->getChildren());
	}
	void build(Vector<NodePtr>& collection) {
		for (auto n : collection) {
			this->matchBuilder(n);
		}
		if (this->expression.empty()) {
			return;
		}
		ShuntingYard sy;
		sy.convert(this->expression);
		collection = this->expression;
	}
public:
	virtual void matchBuilder(NodePtr node) = 0;
public:
	inline ObjectPtr getRoot() {
		return this->root;
	}
	inline TypeManagerPtr getManager() {
		return &this->manager;
	}
public:
	Vector<NodePtr> find(NodePtr node, Filter filter, Logic depth = false);
	void find(Vector<NodePtr>* vector, NodePtr node, Filter filter, Logic depth = false);
	ObjectPtr push(NodePtr node);
	void pop(ObjectPtr obj);
protected:
	inline NodePtr read() {
		if (this->expression.empty()) {
			throw new SyntaxError(SyntaxError::TOKEN_MISMATCH);
		}
		NodePtr top = this->expression.back();
		this->expression.pop_back();
		return top;
	}
protected:
	ObjectPtr root;
	Stack<ObjectPtr> stack;
	ObjectPtr parent;
	Vector<NodePtr> expression;
	TypeManager manager;
} *ScopeBuilderPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__SCOPE_BUILDER__