#include "StackVar.h"
#include "Variable.h"

LAME_BEGIN2(Script)

Void StackVar::Push(ObjectPtr var) {

	this->varList.push_back(var);
	this->nodeList.push_back(var->GetNode());
}

ObjectPtr StackVar::Pop(Void) {

	if (this->varList.empty()) {
		return NULL;
	}

	ObjectPtr var = this->varList.back();

	this->varList.pop_back();
	this->nodeList.pop_back();

	return var;
}

Void StackVar::Clear() {

	while (this->Size()) {
		this->Pop();
	}
}

LAME_END2