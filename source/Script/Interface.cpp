#include "Interface.h"
#include "ScopeController.h"
#include "Class.h"

LAME_BEGIN2(Script)

Error Interface::Implement(InterfacePtr object) {

	Vector<InterfacePtr>::iterator i;

	if ((i = std::find(this->implemented.begin(), this->implemented.end(), object)) != this->implemented.end()) {

		if (*i == this) {
			return Error::Class_AlreadyImplemented;
		}
	}

	this->implemented.push_back(object);

	if (!object->CheckType(Type::Interface)) {
		return Error::Interface_UnableToImplementClass;
	}

	this->GetScopeController()
		->GetMethodScope()->Merge(object->GetScopeController()
		->GetMethodScope());

	return Error::NoError;
}

Void Interface::Trace(Uint32 offset) {

	printf("interface %s ", this->GetName().data());

	if (this->implemented.size() > 0) {

		printf("implements ");

		for (InterfacePtr c : this->implemented) {
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
