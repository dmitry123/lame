#include "Method.h"
#include "Lex.h"
#include "Variable.h"
#include "Class.h"

LAME_BEGIN2(Script)

Method::Method(BufferRefC name, ScopePtr parent, ObjectPtr thisClass, ObjectPtr returnClass, Vector<ClassPtr> attributes) : Object(name, parent, Type::Method),
	attributesHash(attributes),
	thisClass((VariablePtr)thisClass),
	returnClass((ClassPtr) returnClass),
	nativeMethod(0),
	rootNode(0),
	invokeHash(0)
{
	this->invokeHash =
		this->ComputeInvokeHash(attributes);
}

ClassPtr Method::GetClass() {
	return thisClass->GetVariable()->GetObject();
}

MethodPtr Method::GetMethod() {
	return this;
}

Bool Method::Equal(ObjectPtrC object) {
	return this->Hash() == object->Hash();
}

ObjectPtr Method::Clone(BufferRefC name) {
	return this;
}

Void Method::Trace(Uint32 offset) {

	this->PrintModificators();

	printf("%s %s::%s(",
		this->returnClass->GetName().data(),
		this->thisClass->GetName().data(),
		this->GetName().data());

	for (Uint32 i = 0; i < attributesHash.size(); i++) {
		if (i == attributesHash.size() - 1) {
			printf("%s", attributesHash[i]->GetName().data());
		}
		else {
			printf("%s, ", attributesHash[i]->GetName().data());
		}
	}

	printf(")");

	if (this->rootNode) {
		printf(" {");
		this->Scope::Trace(offset + 1);
		Lex::PrintLine(offset);
		printf("}");
	}
	else {
		if (this->GetNativeMethod()) {
			printf(" = native;");
		}
		else {
			printf(" = 0;");
		}
	}
}

Method::HashType Method::Hash(Void) {
	return Uint64(this->GetPathHash32()) << 32 | this->GetInvokeHash();
}

Uint32 Method::Size(Void) {
	return SizeOf / 1;
}

Void Method::Release(Void) {
	// Donkey :D
}

Void Method::SetNativeMethod(NativeMethod method) {

	this->nativeMethod = method;
	this->SetModificator(Modificator::Native);
}

Void Method::Invoke(Vector<VariablePtr> attributes) {

}

Uint32 Method::ComputeInvokeHash(Vector<ClassPtr>& classList) {

	Uint32 invokeHash = 0;

	for (ClassPtr c : classList) {
		invokeHash = 31 * invokeHash + c->GetHash32();
	}

	return invokeHash;
}

Uint32 Method::ComputeInvokeHash(Vector<VariablePtr>& classList) {

	Uint32 invokeHash = 0;

	for (VariablePtr c : classList) {
		invokeHash = 31 * invokeHash + c->GetClass()->GetHash32();
	}

	return invokeHash;
}

LAME_END2
