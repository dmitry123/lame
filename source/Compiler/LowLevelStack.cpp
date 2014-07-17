#include "LowLevelStack.h"

LAME_BEGIN2(Compiler)

LowLevelStack::LowLevelStack() :
	registerScope("Register", 0)
{
	using namespace Script;

	if (!Scope::classInt) {
		throw ScriptException("Initialize root scope first");
	}

	this->r0 = RegisterPtr(this->registerScope.Add(new Register("r0", Scope::classInt)));
	this->r1 = RegisterPtr(this->registerScope.Add(new Register("r1", Scope::classInt)));
	this->r2 = RegisterPtr(this->registerScope.Add(new Register("r2", Scope::classInt)));
	this->r3 = RegisterPtr(this->registerScope.Add(new Register("r3", Scope::classInt)));
	this->r4 = RegisterPtr(this->registerScope.Add(new Register("r4", Scope::classInt)));
	this->r5 = RegisterPtr(this->registerScope.Add(new Register("r5", Scope::classInt)));
	this->r6 = RegisterPtr(this->registerScope.Add(new Register("r6", Scope::classInt)));
	this->r7 = RegisterPtr(this->registerScope.Add(new Register("r7", Scope::classInt)));
	this->r8 = RegisterPtr(this->registerScope.Add(new Register("r8", Scope::classInt)));
	this->r9 = RegisterPtr(this->registerScope.Add(new Register("r9", Scope::classInt)));

	this->r0->SetModificator(Class::Modificator::Register);
	this->r1->SetModificator(Class::Modificator::Register);
	this->r2->SetModificator(Class::Modificator::Register);
	this->r3->SetModificator(Class::Modificator::Register);
	this->r4->SetModificator(Class::Modificator::Register);
	this->r5->SetModificator(Class::Modificator::Register);
	this->r6->SetModificator(Class::Modificator::Register);
	this->r7->SetModificator(Class::Modificator::Register);
	this->r8->SetModificator(Class::Modificator::Register);
	this->r9->SetModificator(Class::Modificator::Register);

	this->iRegList.insert(this->r0);
	this->iRegList.insert(this->r1);
	this->iRegList.insert(this->r2);
	this->iRegList.insert(this->r3);
	this->iRegList.insert(this->r4);
	this->iRegList.insert(this->r5);
	this->iRegList.insert(this->r6);
	this->iRegList.insert(this->r7);
	this->iRegList.insert(this->r8);
	this->iRegList.insert(this->r9);

	this->f0 = RegisterPtr(this->registerScope.Add(new Register("f0", Scope::classFloat)));
	this->f1 = RegisterPtr(this->registerScope.Add(new Register("f1", Scope::classFloat)));
	this->f2 = RegisterPtr(this->registerScope.Add(new Register("f2", Scope::classFloat)));
	this->f3 = RegisterPtr(this->registerScope.Add(new Register("f3", Scope::classFloat)));
	this->f4 = RegisterPtr(this->registerScope.Add(new Register("f4", Scope::classFloat)));
	this->f5 = RegisterPtr(this->registerScope.Add(new Register("f5", Scope::classFloat)));
	this->f6 = RegisterPtr(this->registerScope.Add(new Register("f6", Scope::classFloat)));
	this->f7 = RegisterPtr(this->registerScope.Add(new Register("f7", Scope::classFloat)));
	this->f8 = RegisterPtr(this->registerScope.Add(new Register("f8", Scope::classFloat)));
	this->f9 = RegisterPtr(this->registerScope.Add(new Register("f9", Scope::classFloat)));

	this->f0->SetModificator(Class::Modificator::Register);
	this->f1->SetModificator(Class::Modificator::Register);
	this->f2->SetModificator(Class::Modificator::Register);
	this->f3->SetModificator(Class::Modificator::Register);
	this->f4->SetModificator(Class::Modificator::Register);
	this->f5->SetModificator(Class::Modificator::Register);
	this->f6->SetModificator(Class::Modificator::Register);
	this->f7->SetModificator(Class::Modificator::Register);
	this->f8->SetModificator(Class::Modificator::Register);
	this->f9->SetModificator(Class::Modificator::Register);

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

	this->p0 = RegisterPtr(this->registerScope.Add(new Register("p0", Scope::classObject)));
	this->p1 = RegisterPtr(this->registerScope.Add(new Register("p1", Scope::classObject)));
	this->p2 = RegisterPtr(this->registerScope.Add(new Register("p2", Scope::classObject)));
	this->p3 = RegisterPtr(this->registerScope.Add(new Register("p3", Scope::classObject)));
	this->p4 = RegisterPtr(this->registerScope.Add(new Register("p4", Scope::classObject)));
	this->p5 = RegisterPtr(this->registerScope.Add(new Register("p5", Scope::classObject)));
	this->p6 = RegisterPtr(this->registerScope.Add(new Register("p6", Scope::classObject)));
	this->p7 = RegisterPtr(this->registerScope.Add(new Register("p7", Scope::classObject)));
	this->p8 = RegisterPtr(this->registerScope.Add(new Register("p8", Scope::classObject)));
	this->p9 = RegisterPtr(this->registerScope.Add(new Register("p9", Scope::classObject)));

	this->p0->SetModificator(Class::Modificator::Register);
	this->p1->SetModificator(Class::Modificator::Register);
	this->p2->SetModificator(Class::Modificator::Register);
	this->p3->SetModificator(Class::Modificator::Register);
	this->p4->SetModificator(Class::Modificator::Register);
	this->p5->SetModificator(Class::Modificator::Register);
	this->p6->SetModificator(Class::Modificator::Register);
	this->p7->SetModificator(Class::Modificator::Register);
	this->p8->SetModificator(Class::Modificator::Register);
	this->p9->SetModificator(Class::Modificator::Register);

	this->pRegList.insert(this->p0);
	this->pRegList.insert(this->p1);
	this->pRegList.insert(this->p2);
	this->pRegList.insert(this->p3);
	this->pRegList.insert(this->p4);
	this->pRegList.insert(this->p5);
	this->pRegList.insert(this->p6);
	this->pRegList.insert(this->p7);
	this->pRegList.insert(this->p8);
	this->pRegList.insert(this->p9);
}

RegisterPtr LowLevelStack::FindRegister(Type type) {

	Core::Set<RegisterPtr>& list =
		type == Type::Float   ? this->fRegList :
		type == Type::Integer ? this->iRegList : this->pRegList;

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
		for (RegisterPtr r : this->pRegList) {
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