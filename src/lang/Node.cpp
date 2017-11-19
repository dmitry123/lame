#include "Node.h"

LAME_LANG_BEGIN

Node::~Node() {

	for (NodePtr node : this->children) {
		delete node;
	}
	this->children.clear();

	if (this->word != nullptr) {
		delete this->word;
	}
}

void Node::insert(NodePtr child) {

	if (child == this) {
		return;
	}

	auto it = std::find(this->children.begin(), this->children.end(), child);

	if (it != this->children.end()) {
		return ;
	}

	this->children.push_back(child);

	if (child->parent != nullptr && child->parent != this) {
		child->parent->erase(child);
	}

	child->parent = this;
}

void Node::erase(NodePtr child) {

	if (child == this) {
		return;
	}

	auto it = std::find(this->children.begin(), this->children.end(), child);

	if (it != this->children.end()) {
		this->children.erase(it);
	}
	else {
		return ;
	}

	child->parent = nullptr;
}

NodePtr Node::findByToken(TokenPtr token, Logic depth) {

	NodePtr found = nullptr;

	for (NodePtr node : this->children) {
		if (node->token == token) {
			return node;
		}
		else if (depth == true && (found = node->findByToken(token, depth))) {
			return found;
		}
	}

	return nullptr;
}

NodePtr Node::findByWord(StringPtrC word, Logic depth) {

	NodePtr found = nullptr;

	for (NodePtr node : this->children) {
		if (stricmp(node->word, word) == 0) {
			return node;
		}
		else if (depth == true && (found = node->findByToken(token, depth))) {
			return found;
		}
	}

	return nullptr;
}

LAME_LANG_END