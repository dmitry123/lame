#include "Walker.h"

LAME_LANG_BEGIN

const Vector<NodePtr>& Walker::walk(NodePtr root) {

	assert(this->filter);

	if (root->getToken() != nullptr && this->filter(root)) {
		this->vector.push_back(root);
	}

	for (auto node : root->getChildren()) {
		this->walk(node);
	}

	return this->vector;
}


LAME_LANG_END