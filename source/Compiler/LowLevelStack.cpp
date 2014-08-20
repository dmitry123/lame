#include "LowLevelStack.h"

LAME_BEGIN2(Compiler)

LowLevelStack::LowLevelStack(ScopePtr rootScope) {

	using namespace Script;

	if (!Scope::classInt) {
		throw ScriptException("Initialize root scope first");
	}

	this->registerClass = new Method("register", rootScope, Scope::classVoid, Scope::classVoid);

	this->i0 = RegisterPtr(registerClass->Add(new Register("i0", Scope::classInt)));
	this->i1 = RegisterPtr(registerClass->Add(new Register("i1", Scope::classInt)));
	this->i2 = RegisterPtr(registerClass->Add(new Register("i2", Scope::classInt)));
	this->i3 = RegisterPtr(registerClass->Add(new Register("i3", Scope::classInt)));
	this->i4 = RegisterPtr(registerClass->Add(new Register("i4", Scope::classInt)));
	this->i5 = RegisterPtr(registerClass->Add(new Register("i5", Scope::classInt)));
	this->i6 = RegisterPtr(registerClass->Add(new Register("i6", Scope::classInt)));
	this->i7 = RegisterPtr(registerClass->Add(new Register("i7", Scope::classInt)));
	this->i8 = RegisterPtr(registerClass->Add(new Register("i8", Scope::classInt)));
	this->i9 = RegisterPtr(registerClass->Add(new Register("i9", Scope::classInt)));

	this->iRegList.insert(this->i0);
	this->iRegList.insert(this->i1);
	this->iRegList.insert(this->i2);
	this->iRegList.insert(this->i3);
	this->iRegList.insert(this->i4);
	this->iRegList.insert(this->i5);
	this->iRegList.insert(this->i6);
	this->iRegList.insert(this->i7);
	this->iRegList.insert(this->i8);
	this->iRegList.insert(this->i9);

	for (RegisterPtr r : this->iRegList) {
		r->SetModificator(Object::Modificator::Register);
	}

	this->f0 = RegisterPtr(registerClass->Add(new Register("f0", Scope::classFloat)));
	this->f1 = RegisterPtr(registerClass->Add(new Register("f1", Scope::classFloat)));
	this->f2 = RegisterPtr(registerClass->Add(new Register("f2", Scope::classFloat)));
	this->f3 = RegisterPtr(registerClass->Add(new Register("f3", Scope::classFloat)));
	this->f4 = RegisterPtr(registerClass->Add(new Register("f4", Scope::classFloat)));
	this->f5 = RegisterPtr(registerClass->Add(new Register("f5", Scope::classFloat)));
	this->f6 = RegisterPtr(registerClass->Add(new Register("f6", Scope::classFloat)));
	this->f7 = RegisterPtr(registerClass->Add(new Register("f7", Scope::classFloat)));
	this->f8 = RegisterPtr(registerClass->Add(new Register("f8", Scope::classFloat)));
	this->f9 = RegisterPtr(registerClass->Add(new Register("f9", Scope::classFloat)));

	this->fRegList.insert(this->f0);
	this->fRegList.insert(this->f1);
	this->fRegList.insert(this->f2);
	this->fRegList.insert(this->f3);
	this->fRegList.insert(this->f4);
	this->fRegList.insert(this->f5);
	this->fRegList.insert(this->f6);
	this->fRegList.insert(this->f7);
	this->fRegList.insert(this->f8);
	this->fRegList.insert(this->f9);

	for (RegisterPtr r : this->fRegList) {
		r->SetModificator(Object::Modificator::Register);
	}

	this->r0 = RegisterPtr(registerClass->Add(new Register("r0", Scope::classObject)));
	this->r1 = RegisterPtr(registerClass->Add(new Register("r1", Scope::classObject)));
	this->r2 = RegisterPtr(registerClass->Add(new Register("r2", Scope::classObject)));
	this->r3 = RegisterPtr(registerClass->Add(new Register("r3", Scope::classObject)));
	this->r4 = RegisterPtr(registerClass->Add(new Register("r4", Scope::classObject)));
	this->r5 = RegisterPtr(registerClass->Add(new Register("r5", Scope::classObject)));
	this->r6 = RegisterPtr(registerClass->Add(new Register("r6", Scope::classObject)));
	this->r7 = RegisterPtr(registerClass->Add(new Register("r7", Scope::classObject)));
	this->r8 = RegisterPtr(registerClass->Add(new Register("r8", Scope::classObject)));
	this->r9 = RegisterPtr(registerClass->Add(new Register("r9", Scope::classObject)));

	this->rRegList.insert(this->r0);
	this->rRegList.insert(this->r1);
	this->rRegList.insert(this->r2);
	this->rRegList.insert(this->r3);
	this->rRegList.insert(this->r4);
	this->rRegList.insert(this->r5);
	this->rRegList.insert(this->r6);
	this->rRegList.insert(this->r7);
	this->rRegList.insert(this->r8);
	this->rRegList.insert(this->r9);

	for (RegisterPtr r : this->rRegList) {
		r->SetModificator(Object::Modificator::Register);
	}
}

RegisterPtr LowLevelStack::FindRegister(Type type) {

	Core::Set<RegisterPtr>& list =
		type == Type::Float   ? this->fRegList :
		type == Type::Integer ? this->iRegList : this->rRegList;

	for (RegisterPtr r : list) {
		if (r->Reserve()) {
			return r;
		}
	}

	return NULL;
}

Void LowLevelStack::ReleaseRegister(RegisterPtr var) {

	if (var == NULL) {
		for (RegisterPtr r : this->iRegList) {
			r->Release();
		}
		for (RegisterPtr r : this->fRegList) {
			r->Release();
		}
		for (RegisterPtr r : this->rRegList) {
			r->Release();
		}
	}
	else {
		var->Release();
	}
}

Void LowLevelStack::Push(VariablePtr var) {
	this->callStack.push(var);
}

Script::VariablePtr LowLevelStack::Pop(Void) {

	if (this->callStack.empty()) {
		throw Core::Exception("Stack is empty, push lost");
	}

	VariablePtr var = this->callStack.top();

	this->callStack.pop();
	return var;
}

LAME_END2