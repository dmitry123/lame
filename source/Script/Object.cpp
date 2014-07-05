#include "Object.h"
#include "ScopeController.h"

LAME_BEGIN2(Script)

Object::Object(Object& object) : Object(object.name, object.type, object.GetNode()) {

	this->modificators_ = object.modificators_;
	this->sizeOf_ = object.modificators_;

	if (object.scopeController_) {
		this->EnableScopeController();
		this->GetScopeController()->Clone(object.scopeController_);
	}

	this->countOfArguments_ = 0;
}

Object::~Object() {

	if (this->isScopeControllerEnabled_) {
		delete this->scopeController_;
	}
}

Bool Object::Equal(ObjectPtrC object) const {

	if (this->type == Type::Class) {
		return this->type == object->type && this->hash == object->hash;
	} else {
		return this->type == object->type;
	}
}

Error Object::Clone(ObjectPtrC object) {

	//object->modificators_ = this->modificators_;
	return Error::NoError;
}

Void Object::Trace(Uint32 offset) {
	// ignore
}

Uint64 Object::Hash(Void) const {
	return (Uint64(this->hash) << 32) | this->hash;
}

ObjectPtr Object::SetModificator(Modificator modificator, Bool state) {

	state ?
		this->modificators_ |=  (Uint32)modificator :
		this->modificators_ &= ~(Uint32)modificator;

	return this;
}

Void Object::PrintModificators(Void) {

	if (this->CheckModificator(Modificator::Public)) {
		printf("public ");
	}
	if (this->CheckModificator(Modificator::Private)) {
		printf("private ");
	}
	if (this->CheckModificator(Modificator::Protected)) {
		printf("protected ");
	}
	if (this->CheckModificator(Modificator::Static)) {
		printf("static ");
	}
	if (this->CheckModificator(Modificator::Final)) {
		printf("final ");
	}
	if (this->CheckModificator(Modificator::Native)) {
		printf("native ");
	}
	if (this->CheckModificator(Modificator::Abstract)) {
		printf("abstract ");
	}
}

Bool Object::EnableScopeController(Void) {

	if (this->scopeController_ != NULL) {
		return FALSE;
	}

	this->scopeController_
		= new ScopeController();

	this->isScopeControllerEnabled_ = TRUE;

	return TRUE;
}

LAME_END2
