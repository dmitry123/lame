#include "ScopeBuilder.h"

LAME_LANG_BEGIN

Vector<NodePtr> ScopeBuilder::find(NodePtr node, Filter filter, Logic depth) {

	Vector<NodePtr> vector;
	this->find(&vector, node, filter, depth);
	return vector;
}

void ScopeBuilder::find(Vector<NodePtr>* vector, NodePtr node, Filter filter, Logic depth) {

	if (node->getToken() != nullptr && filter(node)) {
		vector->push_back(node);
	}

	if (node->getToken() == nullptr || depth == true) {
		for (auto n : node->getChildren()) {
			this->find(vector, n, filter);
		}
	}
}

ObjectPtr ScopeBuilder::push(NodePtr node) {

	ObjectPtr obj = new Object(node, this->parent);

	this->stack.push(this->parent);
	this->parent = obj;

	return obj;
}

void ScopeBuilder::pop(ObjectPtr obj) {

	assert(!this->stack.empty() || this->parent == obj);
	this->parent = this->stack.top();
	this->stack.pop();
}

LAME_LANG_END