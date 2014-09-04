#include "StackVar.h"
#include "Exception.h"
#include "Variable.h"

LAME_BEGIN2(Script)

Void StackVar::Push(VariablePtr var) {

	this->varList.push_back(var);
	this->nodeList.push_back(var->GetNode());
}

VariablePtr StackVar::Pop(Void) {

	if (this->varList.empty()) {
		return NULL;
	}

	VariablePtr var = this->varList.back();

	this->varList.pop_back();
	this->nodeList.pop_back();

	if (var) {
		var->wasInStack = FALSE;
	}

	return var;
}

Void StackVar::Clear() {

	while (this->Size()) {
		this->Pop();
	}
	this->Release();
}

Void StackVar::Return(VariablePtr var) {

	this->varHistory.push_back(var);
	this->Push(var);

	if (var) {
		var->wasInStack = TRUE;
	}
}

Void StackVar::Release(Void) {

	for (VariablePtr var : this->varHistory) {
		if (var) {
			var->wasInStack = FALSE;
		}
	}
	this->varHistory.clear();
}

LAME_END2