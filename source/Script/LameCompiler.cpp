#include "LameCompiler.h"

LAME_BEGIN2(Script)

Void LameCompiler::onBinary(ObjectPtr left, ObjectPtr right) {

	Buffer leftName = left->GetName();
	Buffer rightName = right->GetName();

	if (left->CheckModificator(Object::Modificator::Constant)) {
		leftName = left->GetNode()->word;
	}
	if (right->CheckModificator(Object::Modificator::Constant)) {
		rightName = right->GetNode()->word;
	}

	printf("  %s %s %s\n", this->GetCompiler()->GetCurrentNode()->word.data(),
		leftName.data(), rightName.data());
}

Void LameCompiler::onUnary(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  %s %s\n", this->GetCompiler()->GetCurrentNode()->word.data(),
		objectName.data());
}

Void LameCompiler::onCondition(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  %s %s\n", this->GetCompiler()->GetCurrentNode()->word.data(),
		objectName.data());
}

Void LameCompiler::onReturn(ObjectPtr object) {

	if (object) {
		Buffer objectName = object->GetName();

		if (object->CheckModificator(Object::Modificator::Constant)) {
			objectName = object->GetNode()->word;
		}

		printf("  return %s\n", objectName.data());
	}
	else {
		printf("  leave\n");
	}
}

Void LameCompiler::onLoad(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  load %s\n", objectName.data());
}

Void LameCompiler::onStore(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  store %s\n", objectName.data());
}

Void LameCompiler::onInvoke(ObjectPtr method) {
	printf("  invoke %s\n", method->GetName().data());
}

Void LameCompiler::onCast(ObjectPtr object, ClassPtr type) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	if (object->GetClass()->IsIntegerLike() &&
		type->IsIntegerLike() ||
		object->GetClass()->IsFloatLike() &&
		type->IsFloatLike() ||
		object->GetClass()->IsStringLike() &&
		type->IsStringLike() ||
		object->GetClass()->IsBooleanLike() &&
		type->IsBooleanLike() ||
		object->GetClass()->IsObjectLike() &&
		type->IsObjectLike()
	) {
		return /* Ignore cast */;
	}

	printf("  cast %s -> %s\n", objectName.data(),
		type->GetName().data());
}

Void LameCompiler::onClone(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  clone %s\n", objectName.data());
}

Void LameCompiler::onNew(ObjectPtr object) {

	Buffer objectName = object->GetName();

	if (object->CheckModificator(Object::Modificator::Constant)) {
		objectName = object->GetNode()->word;
	}

	printf("  new %s\n", objectName.data());
}

LAME_END2