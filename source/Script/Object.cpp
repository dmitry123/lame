#include "Object.h"
#include "ScopeController.h"

LAME_BEGIN2(Script)

ScriptObject::ScriptObject(ScriptObject& object) : ScriptObject(object.name, object.type) {

	this->modificators_ = object.modificators_;
	this->sizeOf_ = object.modificators_;

	if (object.scopeController_) {
		this->EnableScopeController();
		this->GetScopeController()->Clone(object.scopeController_);
	}
}

ScriptObject::~ScriptObject() {

	if (this->isScopeControllerEnabled_) {
		delete this->scopeController_;
	}
}

ScriptObjectPtr ScriptObject::SetModificator(Modificator modificator, Bool state) {

	state ?
		this->modificators_ |=  (Uint32)modificator :
		this->modificators_ &= ~(Uint32)modificator;

	return this;
}

Bool ScriptObject::EnableScopeController(Void) {

	if (this->scopeController_ != NULL) {
		return FALSE;
	}

	this->scopeController_
		= new ScriptScopeController();

	this->isScopeControllerEnabled_ = TRUE;

	return TRUE;
}

LAME_END2