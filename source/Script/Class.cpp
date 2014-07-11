#include "Class.h"
#include "Variable.h"
#include "Method.h"
#include "Interface.h"
#include "Exception.h"

LAME_BEGIN2(Script)

Error Class::Clone(ObjectPtrC object) {

	Object::Clone(object);

	this->GetScopeController()->Move(
		object->GetScopeController());

	this->IncRef();

	if (object->GetClass()->DecRef()) {
		delete object;
	}

	return Error::NoError;
}

Void Class::Trace(Uint32 offset) {

	this->PrintModificators();

	if (this->GetTemplateClass()) {
		printf("class %s <%s> ", this->GetName().data(), this->GetTemplateClass()->GetName().data());
	}
	else {
		printf("class %s ", this->GetName().data());
	}

	if (this->extended) {
		printf("extends %s ", this->extended->GetName().data());
	}

	if (this->implemented.size() > 0) {

		printf("implements ");

		for (InterfacePtr c : this->implemented) {

			if (c == this->implemented.front()) {
				break;
			}

			printf("%s, ", c->GetName().data());
		}

		printf("%s ", this->implemented.front()->GetName().data());
	}

	printf("{");

	if (this->GetScopeController()->Amount() > 0) {
		this->GetScopeController()->Trace(offset + 1);
		Lex::PrintLine(offset);
	}

	printf("}");
}

Error Class::New(ObjectPtr object) {

	// check object for variable, cuz we can
	// store class's objects only in variables
	if (!object->CheckType(Type::Variable)) {
		return Error::Class_ObjectNotVariable;
	}

	// get object's variable, its true (checking upper)
	VariablePtr var = object->GetVariable();

	// allocate memory for new class's object
	var->SetObject(new Class(this->GetName()));

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

			// get method from just created object. we have
			// it, cuz we've extende that abstract class
			MethodPtr mThis = var->objectValue->GetScopeController()->GetMethodScope()
				->Find(i.first.first, i.second->GetMethod()->GetInvokeHash())->GetMethod();

			// now we have to check our method for null node,
			// if root node is null, then we method not implemented,
			// but only if we also havn't native method callback
			if (mThis->GetRootNode() == NULL && mThis->GetNativeMethod() == NULL) {
				return Error::AbstractClass_ObjectMustImplementAbstractMethod;
			}

			// we can succesfull copy object's variable
			// to it's method
			mThis->SetThis(var);
		}
	}

	// if we have implemented interfaces, then
	// we must do the same thingsm that with
	// abstract class
	for (InterfacePtr i : this->implemented) {

		// look thought all methods
		for (auto i : i->GetScopeController()->GetMethodScope()->GetStringMap()) {

			// we can succesfull copy object's variable
			// to it's method
			var->objectValue->GetScopeController()->GetMethodScope()
				->Find(i.first.first, i.second->GetMethod()->GetInvokeHash())->GetMethod()->SetThis(var);
		}
	}

	// activate all class'es methods
	for (auto i : var->GetObject()->GetScopeController()->GetMethodScope()->GetStringMap()) {

		// get method from just created object. we have
		// it, cuz we've extende that abstract class
		MethodPtr mThis = var->objectValue->GetScopeController()->GetMethodScope()
			->Find(i.first.first, i.second->GetMethod()->GetInvokeHash())->GetMethod();

		// we can succesfull copy object's variable
		// to it's method
		mThis->SetThis(var);
	}

	return Error::NoError;
}

Error Class::Extend(ClassPtr object) {

	if (this->extended != NULL) {
		return Error::Class_AlreadyExtended;
	}

	this->extended = object;
	this->operators = object->operators;

	if (this->CheckType(Type::Interface)) {
		return Error::NoError;
	}

	this->GetScopeController()
		->Merge(object->GetScopeController());

	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
	if (this->extended &&
		this->extended->CheckModificator(Modificator::Abstract)
		) {
		// walk though all methods
		for (auto i : this->extended->GetScopeController()
			->GetMethodScope()->GetStringMap()
			) {
			MethodPtr m = i.second->GetMethod();

			// if we met not abstract method, then ignore it
			if (!m->CheckModificator(Modificator::Abstract)) {
				continue;
			}

			// if out method has root node, its bad
			// cuz abstract methods can't be implemented
			// its stupid fact from Java
			if (m->GetRootNode() != NULL) {
				return Error::AbstractClass_CantStoreImplementedAbstractMethods;
			}
		}
	}

	for (InterfacePtr i : object->implemented) {
		this->Implement(i);
	}

	return Error::NoError;
}

Error Class::Implement(InterfacePtr object) {

	if (std::find(this->implemented.begin(), this->implemented.end(), object) != this->implemented.end()) {
		return Error::Class_AlreadyImplemented;
	}

	if (!object || !object->CheckType(Type::Interface)) {
		return Error::Class_CanImplementOnlyInterfaces;
	}

	this->implemented.push_back(object);

	this->GetScopeController()->GetMethodScope()
		->Merge(object->GetScopeController()->GetMethodScope());

	// look thought all methods
	for (auto i : object->GetScopeController()->GetMethodScope()->GetStringMap()) {

		MethodPtr m = (MethodPtr)i.second;

		// if method has root node then
		// throw an error, cuz interface's
		// methods mustn't be implemented
		if (m->GetRootNode() != NULL) {
			return Error::Interface_MethodsMustBeImplemented;
		}

		ObjectPtr founded;

		// now lets try to find overloaded methods
		// in current class (this), if we cant, then
		// throw an error, cuz class, which implements
		// interface have to implement it's methods
		if (!(founded = this->GetScopeController()->GetMethodScope()->Find(i.second->GetName(), m->GetInvokeHash()))) {
			return Error::Interface_ClassMustImplementMethods;
		}

		// but if we've found method, but it hasn't root node
		// and hasn't native method callback, then its a mistake
		// throw an error
		if (!founded->GetMethod()->GetRootNode() && !founded->GetMethod()->GetNativeMethod()) {
			return Error::Interface_ClassMustImplementMethods;
		}
	}

	return Error::NoError;
}

Error Class::Overload(Operator op, OperatorCallback callback) {

	if (!((Uint32)op > 0 && (Uint32)op < OPERATOR_AMOUNT)) {
		return Error::Class_WrongOperator;
	}

	if (!operators.size()) {
		for (Uint32 i = 0; i < OPERATOR_AMOUNT; i++) {
			operators.push_back(NULL);
		}
	}

	operators.at((Uint32)op) = callback;

	return Error::NoError;
}

Error Class::Evaluate(Operator op, VariablePtr source, VariablePtr left, VariablePtr right, LexPtrC lex) {

	if (!left->CheckType(Type::Variable) && !left->CheckType(Type::Array) ||
		!right->CheckType(Type::Variable) && !right->CheckType(Type::Array)
		) {
		return Error::Class_ObjectNotVariable;
	}

	if (left->GetClass()->GetNameHash() != right->GetClass()->GetNameHash() &&
		left->GetClass()->GetNameHash() != this->GetNameHash()
	) {
		if (
			!left->GetClass()->CheckModificator(Modificator::Primitive) ||
			!right->GetClass()->CheckModificator(Modificator::Primitive)
		) {
			return Error::Class_ObjectsHaveDifferentHash;
		}
	}

	if ((Uint32)op >= OPERATOR_AMOUNT) {
		return Error::Class_WrongOperator;
	}

	if (right->CheckModificator(Modificator::Register) && right->registerType) {
		right = right->registerType->GetVariable();
	}

	if (left->GetClass()->CheckModificator(Modificator::Primitive) &&
		right->GetVariable() && left->GetVariable() && right->GetVariable()->registerType &&
		right->GetVariable()->registerType->GetClass() != left->GetClass()
		) {
		throw ClassInvalidCastException();
	}

	if (left->GetVariable()->GetVarType() == Variable::Var::Object) {
		if (!this->operators.at((Uint32)Operator::Cast)) {
			return Error::Class_OperatorNotOverloaded;
		}
		this->operators.at((Uint32)Operator::Cast)(source, right, left);
	}
	else if (left->GetVariable()->GetVarType() != right->GetVariable()->GetVarType()) {
		if (lex->IsRight()) {
			source->Make(Operator::Cast, left, right);
		}
		else {
			if (!this->operators.at((Uint32)Operator::Cast)) {
				return Error::Class_OperatorNotOverloaded;
			}
			this->operators.at((Uint32)Operator::Cast)(source, left, right);
		}
	}

	if (op != Operator::Cast) {
		if (lex->IsRight()) {
			source->Make(op, left, right);
		}
		else {
			if (!this->operators.at((Uint32)op)) {
				return Error::Class_OperatorNotOverloaded;
			}
			this->operators.at((Uint32)op)(source, right, left);
		}
	}

	return Error::NoError;
}

LAME_END2
