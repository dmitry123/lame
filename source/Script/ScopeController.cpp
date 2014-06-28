#include "ScopeController.h"

LAME_BEGIN2(Script)

ScriptScopePtr ScriptScopeController::Get(Type type) {

	switch (type)
	{
	case Type::Class:
		return this->GetClassScope();
	case Type::Variable:
		return this->GetVarScope();
	case Type::Method:
		return this->GetMethodScope();
	default:
		return NULL;
	}
}

Void ScriptScopeController::Merge(ScriptScopeControllerPtr sc) {

	this->GetClassScope()->Merge(sc->GetClassScope());
	this->GetVarScope()->Merge(sc->GetVarScope());
	this->GetMethodScope()->Merge(sc->GetMethodScope());
}

Void ScriptScopeController::Clone(ScriptScopeControllerPtr sc) {

	this->GetClassScope()->Clone(sc->GetClassScope());
	this->GetVarScope()->Clone(sc->GetVarScope());
	this->GetMethodScope()->Clone(sc->GetMethodScope());
}

Uint32 ScriptScopeController::Amount(Void) {

	return
		this->GetClassScope()->Amount() +
		this->GetMethodScope()->Amount() +
		this->GetVarScope()->Amount();
}

Void ScriptScopeController::Trace(Uint32 offset) {

	this->GetClassScope()->Trace(offset);

	if (this->GetClassScope()->Amount() > 0) {
		puts("");
	}

	this->GetMethodScope()->Trace(offset);

	if (this->GetMethodScope()->Amount() > 0) {
		puts("");
	}

	this->GetVarScope()->Trace(offset);
}

Void ScriptScopeController::Clear(Void) {

	this->GetClassScope()->Clear();
	this->GetMethodScope()->Clear();
	this->GetVarScope()->Clear();
	this->GetTempScope()->Clear();
}

LAME_END2