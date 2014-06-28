#include "Method.h"
#include "ScopeController.h"
#include "Variable.h"
#include "Class.h"

LAME_BEGIN2(Script)

ScriptMethod::ScriptMethod(
		BufferRefC methodName,
		ScriptObjectPtr thisClass,
		ScriptObjectPtr returnClass,
		Vector<Uint32> attributes) : ScriptObject(methodName, Type::Method),

	attributesHash(attributes),
	thisClass((ScriptVarPtr)thisClass),
	returnClass((ScriptClassPtr) returnClass),
	nativeMethod(0),
	rootNode(0)
{
	this->SetModificator(Modificator::Final);
	this->EnableScopeController();
}

ScriptClassPtr ScriptMethod::GetClass() {
	return thisClass->GetObject();
}

Void ScriptMethod::Trace(Uint32 offset) {

	printf("%s %s::%s(",
		this->returnClass->GetName().data(),
		this->thisClass->GetName().data(),
		this->GetName().data());

	for (Uint32 hash : attributesHash) {
		printf("%d, ", hash);
	}

	printf(")");

	if (this->rootNode) {
		printf(" {}");
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

ScriptError ScriptMethod::SetNativeMethod(NativeMethod method) {

	if (this->nativeMethod != NULL) {
		//return ScriptError::Method_CantOverrideNativeMethod;
	}

	this->nativeMethod = method;
	this->SetModificator(Modificator::Native);

	return ScriptError::NoError;
}

ScriptError ScriptMethod::Invoke(Vector<ScriptVarPtr> attributes) {

	if (attributes.size() != this->attributesHash.size()) {
		return ScriptError::Method_InvalidArguments;
	}

	ScriptScopeControllerPtr scopes
		= this->GetScopeController();
	
	if (!scopes->Amount()) {

		if (!this->thisClass && !this->CheckModificator(Modificator::Static)) {
			return ScriptError::Class_NotStaticObjectWithoutThis;
		}

		if (this->thisClass) {
			scopes->GetVarScope()->Add(new ScriptVar("this", this->thisClass))
				->SetModificator(Modificator::Final)->Clone(this->GetThis());
		}

		for (ScriptVarPtr var : attributes) {
			scopes->GetVarScope()->Add(new ScriptVar(var->GetName().data(), var->GetClass()))
				->Clone(var);
		}
	}

	if (this->nativeMethod != NULL) {
		this->nativeMethod(this);
	} else {
		// invoke script method
	}

	return ScriptError::NoError;
}

LAME_END2