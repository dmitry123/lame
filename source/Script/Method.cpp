#include "Method.h"
#include "ScopeController.h"
#include "Variable.h"
#include "Class.h"
#include "Performer.h"

LAME_BEGIN2(Script)

Method::Method(
		BufferRefC methodName,
		NodePtr node,
		ObjectPtr thisClass,
		ObjectPtr returnClass,
		Vector<ClassPtr> attributes) : Object(methodName, Type::Method, node),

	attributesHash(attributes),
	thisClass((VariablePtr)thisClass),
	returnClass((ClassPtr) returnClass),
	nativeMethod(0),
	rootNode(0),
	invokeHash(0)
{
	this->EnableScopeController();

	this->invokeHash =
		this->ComputeInvokeHash(attributes);
}

ClassPtr Method::GetClass() {
	return thisClass->GetObject();
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
		this->GetScopeController()->Trace(offset + 1);
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

Uint64 Method::Hash(Void) const {
	return Uint64(this->GetNameHash()) << 32 | this->invokeHash;
}

Error Method::SetNativeMethod(NativeMethod method) {

	if (this->nativeMethod != NULL) {
		//return Error::Method_CantOverrideNativeMethod;
	}

	this->nativeMethod = method;
	this->SetModificator(Modificator::Native);

	return Error::NoError;
}

Error Method::Invoke(NodePerformerPtr performer, Vector<VariablePtr> attributes) {

	if (attributes.size() != this->attributesHash.size()) {
		return Error::Method_InvalidArguments;
	}

	for (Uint32 i = 0; i < this->attributesHash.size(); i++) {
		if (  this->attributesHash[i]->GetNameHash() !=
			attributes[i]->GetClass()->GetNameHash()
		) {
			return Error::Method_InvalidArguments;
		}
	}

	ScopeControllerPtr scopes
		= this->GetNode()->var->GetScopeController();

	if (!this->thisClass && !this->CheckModificator(Modificator::Static)) {
		return Error::Class_NotStaticObjectWithoutThis;
	}

	if (this->thisClass) {

		ObjectPtr thisObject = scopes->GetVarScope()
			->Add(new Variable("this", this->thisClass))
			->SetModificator(Modificator::Internal);

		if (thisObject) {
			this->GetThis()->Clone(thisObject);
		}
	}

	for (Uint32 i = 0; i < attributes.size(); i++) {
		attributes[attributes.size() - 1 - i]->Clone(
			scopes->GetVarScope()->Find(this->GetNode()->argList[i]->word.data()));
	}

	if (this->nativeMethod != NULL) {
		this->nativeMethod(this);
	} else if (performer) {
		performer->Evaluate(this->GetRootNode()->blockList);
	}

	return Error::NoError;
}

Bool Method::CompareArguments(MethodPtr method) {

	if (method->attributesHash.size() != this->attributesHash.size()) {
		return FALSE;
	}

	for (Uint32 i = 0; i < this->attributesHash.size(); i++) {
		if (  this->attributesHash[i]->GetNameHash() !=
			method->attributesHash[i]->GetClass()->GetNameHash()
		) {
			return FALSE;
		}
	}

	return TRUE;
}

Uint32 Method::ComputeInvokeHash(Vector<ClassPtr>& classList) {

	Uint32 invokeHash = 0;

	for (ClassPtr c : classList) {
		invokeHash = 31 * invokeHash + c->GetNameHash();
	}

	return invokeHash;
}

Uint32 Method::ComputeInvokeHash(Vector<VariablePtr>& classList) {

	Uint32 invokeHash = 0;

	for (VariablePtr c : classList) {
		invokeHash = 31 * invokeHash + c->GetClass()->GetNameHash();
	}

	return invokeHash;
}

LAME_END2
