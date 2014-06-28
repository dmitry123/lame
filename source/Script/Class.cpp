#include "Class.h"
#include "Variable.h"
#include "Method.h"
#include "Interface.h"

LAME_BEGIN2(Script)

ScriptError ScriptClass::Clone(ScriptObjectPtrC object) {

	this->GetScopeController()->Clone(
		object->GetScopeController());

	return ScriptError::NoError;
}

ScriptError ScriptClass::Cast(ScriptObjectPtrC object) {

	if (this->GetHash() != object->GetHash() && !object->CheckType(Type::Class)) {
		return ScriptError::Object_UnableToCast;
	}

	if (this == object) {
		return ScriptError::NoError;
	}

	return this->Clone(object);
}

Void ScriptClass::Trace(Uint32 offset) {

	if (this->CheckModificator(Modificator::Abstract)) {
		printf("abstract ");
	}

	printf("class %s ", this->GetName().data(), this->GetReferences());

	if (this->extended) {
		printf("extends %s ", this->extended->GetName().data());
	}

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

ScriptError ScriptClass::New(ScriptObjectPtr object) {

	// check object for variable, cuz we can
	// store class's objects only in variables
	if (!object->CheckType(Type::Variable)) {
		return ScriptError::Class_ObjectNotVariable;
	}

	// get object's variable, its true (checking upper)
	ScriptVarPtr var = object->GetVariable();

	// if variable just had class object, then decrement
	// it's reference and try to release memory
	if (var->objectValue && var->objectValue->DecRef()) {
		delete var->objectValue;
	}

	// allocate memory for new class's object
	var->objectValue = new ScriptClass(this->GetName());

	// copy all scopes and it's script objects
	// to new class variable
	var->objectValue->GetScopeController()
		->Clone(this->GetScopeController());

	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
	if (this->extended != NULL && this->extended->CheckModificator(Modificator::Abstract)) {

		// walk though all methods
		for (auto i : this->extended->GetScopeController()->GetMethodScope()->GetStringMap()) {

			ScriptMethodPtr m = i.second->GetMethod();

			// if we met not abstract method, then ignore it
			if (!m->CheckModificator(Modificator::Abstract)) {
				continue;
			}

			// if out method has root node, its bad
			// cuz abstract methods can't be implemented
			// its stupid fact from Java
			if (m->GetRootNode() != NULL) {
				return ScriptError::AbstractClass_CantStoreImplementedAbstractMethods;
			}

			// get method from just created object. we have
			// it, cuz we've extende that abstract class
			ScriptMethodPtr mThis = var->objectValue->GetScopeController()->GetMethodScope()
				->Find(i.first.first)->GetMethod();

			// now we have to check our method for null node,
			// if root node is null, then we method not implemented,
			// but only if we also havn't native method callback
			if (mThis->GetRootNode() == NULL && mThis->GetNativeMethod() == NULL) {
				return ScriptError::AbstractClass_ObjectMustImplementAbstractMethod;
			}

			// we can succesfull copy object's variable
			// to it's method
			mThis->SetThis(var);
		}
	}

	// if we have implemented interfaces, then
	// we must do the same thingsm that with
	// abstract class
	for (ScriptInterfacePtr i : this->implemented) {

		// look thought all methods
		for (auto i : i->GetScopeController()->GetMethodScope()->GetStringMap()) {

			ScriptMethodPtr m = (ScriptMethodPtr)i.second;

			// if method has root node then
			// throw an error, cuz interface's
			// methods mustn't be implemented
			if (m->GetRootNode() != NULL) {
				return ScriptError::Interface_MethodsMustBeImplemented;
			}

			ScriptObjectPtr founded;

			// now lets try to find overloaded methods
			// in current class (this), if we cant, then
			// throw an error, cuz class, which implements
			// interface have to implement it's methods
			if (!(founded = this->GetScopeController()->GetMethodScope()->Find(i.second->GetName()))) {
				return ScriptError::Interface_ClassMustImplementMethods;
			}

			// but if we've found method, but it hasn't root node
			// and hasn't native method callback, then its a mistake
			// throw an error
			if (!founded->GetMethod()->GetRootNode() && !founded->GetMethod()->GetNativeMethod()) {
				return ScriptError::Interface_ClassMustImplementMethods;
			}

			// we can succesfull copy object's variable
			// to it's method
			var->objectValue->GetScopeController()->GetMethodScope()
				->Find(i.first.first)->GetMethod()->SetThis(var);
		}
	}
	
	return ScriptError::NoError;
}

ScriptError ScriptClass::Extend(ScriptClassPtr object) {

	if (this->extended != NULL) {
		return ScriptError::Class_AlreadyExtended;
	}

	this->extended = object;

	this->GetScopeController()
		->Merge(object->GetScopeController());

	for (ScriptInterfacePtr i : object->implemented) {
		this->Implement(i);
	}

	return ScriptError::NoError;
}

ScriptError ScriptClass::Implement(ScriptInterfacePtr object) {

	if (std::find(this->implemented.begin(), this->implemented.end(), object) != this->implemented.end()) {
		return ScriptError::Class_AlreadyImplemented;
	}

	this->implemented.push_back(object);

	this->GetScopeController()->GetMethodScope()
		->Merge(object->GetScopeController()->GetMethodScope());

	return ScriptError::NoError;
}

ScriptError ScriptClass::Overload(Operator op, OperatorCallback callback) {

	if (!((Uint32)op > 0 && (Uint32)op < OPERATOR_AMOUNT)) {
		return ScriptError::Class_WrongOperator;
	}

	if (!operators.size()) {

		operators.resize(OPERATOR_AMOUNT);

		for (OperatorCallback& oc : operators) {
			oc = NULL;
		}
	}

	operators.at((Uint32) op) = callback;

	return ScriptError::NoError;
}

ScriptError ScriptClass::Evaluate(Operator op, ScriptObjectPtr left, ScriptObjectPtr right) {

	if (!left->CheckType(Type::Variable) || !right->CheckType(Type::Variable)) {
		return ScriptError::Class_ObjectNotVariable;
	}

	if (left->GetClass()->GetHash() != right->GetClass()->GetHash() &&
		left->GetClass()->GetHash() != this->GetHash()
	) {
		return ScriptError::Class_ObjectsHaveDifferentHash;
	}

	if (!((Uint32)op > 0 && (Uint32)op < OPERATOR_AMOUNT)) {
		return ScriptError::Class_WrongOperator;
	}

	if (!operators.at((Uint32)op)) {
		return ScriptError::Class_OperatorNotOverloaded;
	}

	operators.at((Uint32)op)((ScriptVarPtr)left, (ScriptVarPtr)right);

	return ScriptError::NoError;
}

LAME_END2