#include "VariableStack.h"

LAME_BEGIN2(Compiler)

Void VariableStack::Push(VariablePtr var) {
    
    this->varList.push_back(var);
    this->nameList.push_back(var->GetName());
}

VariablePtr VariableStack::Pop(Void) {
    
    if (this->varList.empty()) {
        throw ScriptException("VariableStack : Unable to pop variable, stack is empty");
    }
    
    VariablePtr var = this->varList.back();
    
    this->varList.pop_back();
    this->nameList.pop_back();
    
    var->wasInStack = FALSE;
    
    return var;
}

Void VariableStack::Clear() {

	while (this->Size()) {
		this->Pop();
	}
}

LAME_END2