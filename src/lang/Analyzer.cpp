#include "Analyzer.h"

LAME_LANG_BEGIN

static void find(Vector<ObjectPtr>* list, ObjectPtr root, Logic(*filter)(ObjectPtr obj)) {

	if (filter(root)) {
		list->push_back(root);
	}

	for (auto obj : root->getChildren()) {
		find(list, obj.second, filter);
	}
}

Vector<ObjectPtr> Analyzer::findClasses(ObjectPtr root) {

	Vector<ObjectPtr> list;

	find(&list, root, [] (ObjectPtr obj) -> Logic {
		return obj->isClass() || obj->isAnnotation() || obj->isInterface();
	});

	return list;
}

Vector<ObjectPtr> Analyzer::findMethods(ObjectPtr root) {

	Vector<ObjectPtr> list;

	find(&list, root, [] (ObjectPtr obj) -> Logic {
		return obj->isMethod();
	});

	return list;
}

LAME_LANG_END