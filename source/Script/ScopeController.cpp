#include "ScopeController.h"

LAME_BEGIN2(Script)

ScopePtr ScopeController::Get(Type type) {

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

Void ScopeController::Merge(ScopeControllerPtr sc) {

	this->GetClassScope()->Merge(sc->GetClassScope());
	this->GetVarScope()->Merge(sc->GetVarScope());
	this->GetMethodScope()->Merge(sc->GetMethodScope());
}

Void ScopeController::Clone(ScopeControllerPtr sc) {

	this->GetClassScope()->Clone(sc->GetClassScope());
	this->GetVarScope()->Clone(sc->GetVarScope());
	this->GetMethodScope()->Clone(sc->GetMethodScope());
}

Void ScopeController::Move(ScopeControllerPtr sc) {

	this->GetClassScope()->Move(sc->GetClassScope());
	this->GetVarScope()->Move(sc->GetVarScope());
	this->GetMethodScope()->Move(sc->GetMethodScope());
}

Uint32 ScopeController::Amount(Void) {

	return
		this->GetClassScope()->Amount() +
		this->GetMethodScope()->Amount() +
		this->GetVarScope()->Amount();
}

Void ScopeController::Trace(Uint32 offset, Bool temp) {

	this->GetClassScope()->Trace(offset);
	this->GetMethodScope()->Trace(offset);
	this->GetVarScope()->Trace(offset);
}

Void ScopeController::Clear(Void) {

	this->GetClassScope()->Clear();
	this->GetMethodScope()->Clear();
	this->GetVarScope()->Clear();
	this->GetTempScope()->Clear();
}

Uint32 ScopeController::Size(Void) {

	Uint32 size = 0;

	for (auto& i : this->GetMethodScope()->GetStringMap()) {
		if (i.second->GetScopeController()) {
			size += i.second->GetScopeController()->Size();
		}
	}

	for (auto& i : this->GetClassScope()->GetStringMap()) {
		if (i.second->GetScopeController()) {
			size += i.second->GetScopeController()->Size();
		}
	}

	size += this->GetVarScope()->Size();

	for (auto& i : this->GetTempScope()->GetStringMap()) {
		if (i.first.first.length() > 0) {
			size += i.second->GetSizeOf();
		}
	}

	return size;
}

Void ScopeController::Write(Uint8P buffer, Uint32P offset) {

	for (auto& i : this->GetMethodScope()->GetStringMap()) {
		if (i.second->GetScopeController()) {
			i.second->GetScopeController()->Write(buffer + *offset, offset);
		}
	}

	this->GetVarScope()->Write(buffer + *offset, offset);

	for (auto& i : this->GetTempScope()->GetStringMap()) {
		if (i.first.first.length() > 0) {
			i.second->Write(buffer + *offset, offset);
			*offset += i.second->GetSizeOf();
		}
	}
}

LAME_END2
