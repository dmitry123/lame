#include "Object.h"
#include "Randomizer.h"

#include <sstream>

LAME_LANG_BEGIN

Object::~Object() {

	for (auto n : this->children) {
		delete n.second;
	}
}

void Object::insert(ObjectPtr object) {

	if (object == this) {
		return;
	}

	if (object->isConstruction()) {
		object->getNode()->setWord(Randomizer::getDefault()->randomize());
	}

	auto hash = object->calculateHash();
	auto it = this->children.find(hash);

	if (it != this->children.end()) {
		return;
	};

	this->children.insert(std::make_pair(hash, object));

	if (object->parent != nullptr && object->parent != this) {
		object->parent->erase(object);
	}
	else {
		object->parent = this;
	}

	object->hash = hash;
}

Logic Object::erase(ObjectPtr object) {

	if (object == this) {
		return false;
	}

	auto it = this->children.find(object->getHash());

	if (it != this->children.end()) {
		this->children.erase(it);
	}
	else {
		return false;
	}

	object->parent = nullptr;
	object->hash = object->calculateHash();

	return true;
}

ObjectPtr Object::find(Hash hash, Logic depth) {

	auto it = this->children.find(hash);

	if (it != this->children.end()) {
		return it->second;
	}
	
	return nullptr;
}

ObjectPtr Object::find(StringPtrC name, Logic depth) {

	ObjectPtr found = nullptr;

	for (auto p : this->children) {
		if (!strcmp(p.second->node->getWord(), name)) {
			return p.second;
		}
		else if (depth == true) {
			if ((found = p.second->find(name, depth)) != nullptr) {
				return found;
			}
		}
	}

	return nullptr;
}

void Object::merge(ObjectPtr object) {

	for (auto n : object->children) {
		n.second->clone(this);
	}
}

ObjectPtr Object::clone(ObjectPtr parent) {

	ObjectPtr object = new Object(this->node, parent);

	for (auto n : this->children) {
		n.second->clone(object);
	}

	return object;
}

void Object::move(ObjectPtr object) {

	for (auto n : this->children) {
		object->insert(n.second);
	}
}

UInt32 Object::count() {

	return this->children.size();
}

void Object::clear() {

	this->children.clear();
}

StringPtrC Object::path() {

	if (this->cache != nullptr) {
		return nullptr;
	}

	Stack<ObjectPtr> stack;
	ObjectPtr parent = this;

	while (parent != nullptr) {
		stack.push(parent);
		parent = parent->parent;
	}

	std::stringstream ss;

	while (stack.empty()) {
		ss << stack.top() << '/';
		stack.pop();
	}

	this->cache = strcpy(new char[ss.str().length() + 1], ss.str().data());

	return StringPtrC(this->cache);
}

ObjectPtr Object::root() {

	ObjectPtr root = this;

	while (root->parent != nullptr) {
		root = root->parent;
	}

	return root;
}

Object::Hash Object::calculateHash() {

	UInt32 part = 0;

	auto node = this->node;

	if (node == nullptr) {
		return 0;
	}

	if (node->getIdentifier() != nullptr) {
		node = node->getIdentifier();
	}

	if (node->is(Node::FLAG_METHOD)) {
		for (auto n : node->getArgs()) {
			part ^= MurmurHash::calculate32(n->getWord());
		}
	}

	if (this->cache != nullptr) {
		delete this->cache;
	}
	this->cache = nullptr;

	if (this->parent != nullptr && this->parent->node != nullptr) {
		return (Hash) MurmurHash::calculate32(node->getWord(),
			MurmurHash::calculate32(this->parent->node->getWord())
		) | UInt64(part) << 32;
	}
	else {
		return (Hash) MurmurHash::calculate32(node->getWord()) |
			UInt64(part) << 32;
	}
	
	return 0;
}

LAME_LANG_END