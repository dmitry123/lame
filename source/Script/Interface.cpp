#include "Interface.h"
#include "Exception.h"
#include "Class.h"

LAME_BEGIN2(Script)

Interface::Interface(BufferRefC name, ScopePtr parent) : Object(name, parent, Type::Interface),
	inheritor(0)
{
}

Void Interface::Implement(InterfacePtr object) {

	Vector<InterfacePtr>::iterator i;

	if ((i = std::find(this->implemented.begin(), this->implemented.end(), object)) != this->implemented.end()) {

		if (*i == this) {
			throw InterfaceException("Interface has already implement that interface");
		}
	}

	this->implemented.push_back(object);

	if (!object->CheckType(Type::Interface)) {
		throw InterfaceException("Unable to implement non-interface object");
	}

	this->Merge(object);
}

Bool Interface::Equal(ObjectPtrC object) {
	return this->Hash() == object->Hash();
}

ObjectPtr Interface::Clone(BufferRefC name, ObjectPtr parent) {

	InterfacePtr newInterface
		= new Interface(name, parent);

	newInterface->Move(this);

	return newInterface;
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

	if (this->Amount() > 0) {
		puts(""); this->Trace(offset + 1);
	}

	printf("}");
}

Interface::HashType Interface::Hash(Void) {
	return this->GetPathHash64();
}

Uint32 Interface::Size(Void) {
	return 0;
}

Void Interface::Release(Void) {
	// Donkey
}

LAME_END2
