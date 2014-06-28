#include "Interface.h"
#include "ScopeController.h"

LAME_BEGIN2(Script)

ScriptError ScriptInterface::Implement(ScriptInterfacePtr object) {

	Vector<ScriptInterfacePtr>::iterator i;

	if ((i = std::find(this->implemented.begin(), this->implemented.end(), object)) != this->implemented.end()) {

		if (*i == this) {
			return ScriptError::Class_AlreadyImplemented;
		}
	}

	this->implemented.push_back(object);

	if (!object->CheckType(Type::Interface)) {
		return ScriptError::Interface_UnableToImplementClass;
	}

	this->GetScopeController()
		->GetMethodScope()->Merge(object->GetScopeController()
		->GetMethodScope());

	return ScriptError::NoError;
}

Void ScriptInterface::Trace(Uint32 offset) {

	printf("interface %s ", this->GetName().data(), this->GetHash());

	if (this->implemented.size() > 0) {

		printf("implements ");

		for (ScriptInterfacePtr c : this->implemented) {
			printf("%s, ", c->GetName().data());
		}

		printf("\b\b ");
	}

	printf("{");

	if (this->GetScopeController()->Amount() > 0) {
		puts(""); this->GetScopeController()->Trace(offset + 1);
	}

	printf("}");
}

LAME_END2