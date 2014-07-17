#include "Class.h"
#include "Lex.h"
#include "Exception.h"
#include "Variable.h"
#include "Method.h"

LAME_BEGIN2(Script)

Class::Class(BufferRefC name, ScopePtr parent, Uint32 size) : Object(name, parent, Type::Class),
	extendClass(0),
	priority(0),
	size(size)
{
	this->references = new Uint32(0);
}

Class::~Class() {

	if (!(*this->references)) {
		delete this->references;
	}
}

Bool Class::Equal(ObjectPtrC object) {
	return this->Hash() == object->Hash();
}

ObjectPtr Class::Clone(BufferRefC name, ObjectPtr parent) {

	ClassPtr newClass =
		new Class(name, parent);

	this->Move(newClass);

	return newClass;
}

Void Class::Trace(Uint32 offset) {

	this->PrintModificators();

	if (this->GetTemplateClass()) {
		printf("class %s <%s> ", this->GetName().data(), this->GetTemplateClass()->GetName().data());
	}
	else {
		printf("class %s ", this->GetName().data());
	}

	if (this->extendClass) {
		printf("extends %s ", this->extendClass->GetName().data());
	}

	if (this->implementClass.size() > 0) {

		printf("implements ");

		for (ObjectPtr c : this->implementClass) {
			printf("%s, ", c->GetName().data());
		}
	}

	printf("{");

	if (this->Amount() > 0) {
		this->Scope::Trace(offset + 1);
		Lex::PrintLine(offset);
	}

	printf("}");
}

Class::HashType Class::Hash(Void) {
	return this->GetPathHash64();
}

Uint32 Class::Size(Void) {
	return this->Scope::Size();
}

Void Class::Release(Void) {
	if (this->DecRef()) {
		delete this;
	}
}

ClassPtr Class::GetClass(Void) {
	return this;
}

Void Class::New(ObjectPtr object) {

	// check object for variable, cuz we can
	// store class's objects only in variables
	if (!object->CheckType(Type::Variable)) {
		throw ScriptException("Unable to apply operator new for non-variable object");
	}

	// get object's variable, its true (checking upper)
	VariablePtr var = object->GetVariable();
	var->SetObject(new Class(this->GetName(), object->GetParent()));
	var->objectValue->Scope::Clone(this);

	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
	if (this->extendClass != NULL && this->extendClass->CheckModificator(Modificator::Abstract)) {
		for (ObjectPtr i : this->extendClass->GetMethodSet()) {
			MethodPtr mThis = var->objectValue->Find(i->Hash())->GetMethod();
			LAME_TODO("Add strong abstract methods implement!");
			mThis->SetThis(var);
		}
	}

	// if we have implemented interfaces, then
	// we must do the same thingsm that with
	// abstract class
	for (ObjectPtr i : this->implementClass) {
		for (ObjectPtr i : i->GetMethodSet()) {
			var->objectValue->Find(i->Hash())
				->GetMethod()->SetThis(var);
		}
	}

	// activate all class'es methods
	for (ObjectPtr i : var->GetObject()->GetMethodSet()) {
		MethodPtr mThis = var->objectValue->Find(i->Hash())->GetMethod();
		mThis->SetThis(var);
	}
}

Void Class::Extend(ObjectPtr object) {

	if (this->extendClass != NULL) {
		throw ScriptException("Class has already been extended");
	}

	this->extendClass = object;

	if (this->CheckType(Type::Interface)) {
		return;
	}

	this->Scope::Merge(object);

	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
	if (this->extendClass &&
		this->extendClass->CheckModificator(Modificator::Abstract)
	) {
		// walk though all methods
		for (ObjectPtr i : this->extendClass->GetMethodSet()) {

			MethodPtr m = i->GetMethod();

			if (!m->CheckModificator(Modificator::Abstract)) {
				continue;
			}

			// if out method has root node, its bad
			// cuz abstract methods can't be implemented
			// its stupid fact from Java
			if (m->GetRootNode() != NULL) {
				throw ScriptException("Abstrac class can't implement abstract methods");
			}
		}
	}

	for (ObjectPtr i : object->GetClass()->implementClass) {
		this->Implement(i);
	}
}

Void Class::Implement(ObjectPtr object) {

	if (this->implementClass.count(object)) {
		throw ScriptException("Class has already been implemented with that class");
	}

	if (!object || !object->CheckType(Type::Interface)) {
		throw ScriptException("Class can only implement interfaces");
	}

	this->implementClass.insert(object);
	this->Scope::Merge(object);

	// look thought all methods
	for (auto i : object->GetMethodSet()) {

		MethodPtr m = i->GetMethod();

		// if method has root node then
		// throw an error, cuz interface's
		// methods mustn't be implemented
		if (m->GetRootNode() != NULL) {
			throw ScriptException("Interface methods don't have to be implemented");
		}

		ObjectPtr founded;

		// now lets try to find overloaded methods
		// in current class (this), if we cant, then
		// throw an error, cuz class, which implements
		// interface have to implement it's methods
		if (!(founded = this->Find(i->Hash()))) {
			throw ScriptException("Class must implement interface methods");
		}

		// but if we've found method, but it hasn't root node
		// and hasn't native method callback, then its a mistake
		// throw an error
		if (!founded->GetMethod()->GetRootNode() && !founded->GetMethod()->GetNativeMethod()) {
			throw ScriptException("Class must implement interface methods");
		}
	}
}

LAME_END2
