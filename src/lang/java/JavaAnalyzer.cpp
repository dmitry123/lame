#include "JavaAnalyzer.h"

LAME_LANG_BEGIN_WITH(java)

void Analyzer::analyze() {

	for (auto obj : this->findClasses(this->getBuilder()->getRoot())) {

	}
}

void Analyzer::analyze(const Vector<NodePtr>& vector) {

	for (auto n : vector) {
	}
}

LAME_LANG_END_WITH