#include "TypeManager.h"

LAME_LANG_BEGIN

TypeManager::TypeManager(ObjectPtr scope) {

	if (scope == nullptr) {
		this->scope = new Object(nullptr);
	}
}

ObjectPtr TypeManager::declare(NodePtr node) {

	if (node->getIdentifier() != nullptr) {
		node = node->getIdentifier();
	}

	auto obj = this->scope->find(node->getWord());

	if (obj != nullptr) {
		return obj;
	}

	return new Object(node, this->scope);
}

LAME_LANG_END