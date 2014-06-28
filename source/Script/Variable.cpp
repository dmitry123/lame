#include "Variable.h"
#include "Internal.h"

LAME_BEGIN2(Script)

#define __EvalError(_operation) \
	PostSyntaxError(0, "Unable to apply %s operation to this type", #_operation, left->name.data());

ScriptVar::ScriptVar(BufferRefC name, ScriptObjectPtrC type) : ScriptObject(name, Type::Variable),
	classType((ScriptClassPtrC)type)
{
	this->objectValue = NULL;

	if (!type->CheckModificator(Modificator::Primitive)) {
		this->MakeObject();
	}
}

ScriptVar::~ScriptVar() {

	if (this->objectValue != NULL && this->objectValue->DecRef()) {
		delete this->objectValue;
	}
}

ScriptVarPtr ScriptVar::MakeInteger(ScriptNativeInt i) {

	this->v.intValue = i;
	this->varType = Var::Integer;
	return this;
}

ScriptVarPtr ScriptVar::MakeFloat(ScriptNativeFloat f) {

	this->v.floatValue = f;
	this->varType = Var::Float;
	return this;
}

ScriptVarPtr ScriptVar::MakeObject(ScriptClassPtr c) {

	c->IncRef();
	this->objectValue = c;
	this->varType = Var::Object;
	return this;
}

ScriptVarPtr ScriptVar::MakeInteger(Void) {

	if (this->varType == Var::Integer) {
		return this;
	}

	if (this->varType == Var::Float) {
		this->v.intValue = (ScriptNativeInt) this->v.floatValue;
	}

	this->varType = Var::Integer;

	return this;
}

ScriptVarPtr ScriptVar::MakeFloat(Void) {

	if (this->varType == Var::Float) {
		return this;
	}

	if (this->varType == Var::Integer) {
		this->v.floatValue = (ScriptNativeFloat) this->v.intValue;
	}

	this->varType = Var::Float;

	return this;
}

ScriptVarPtr ScriptVar::MakeObject(Void) {

	if (this->objectValue) {
		this->objectValue->DecRef();
	}
	this->objectValue = NULL;
	this->varType = Var::Object;
	return this;
}

ScriptError ScriptVar::Clone(ScriptObjectPtrC object) {

	if (!object->CheckType(Type::Variable)) {
		return ScriptError::Class_ObjectNotVariable;
	}

	if (object->GetVariable()->varType == Var::Object) {
		this->MakeObject(object->GetVariable()->objectValue);
	}
	else if (object->GetVariable()->varType == Var::Float) {
		this->MakeFloat(object->GetVariable()->v.floatValue);
	}
	else{
		this->MakeInteger(object->GetVariable()->v.intValue);
	}

	return ScriptError::NoError;
}

ScriptError ScriptVar::Cast(ScriptObjectPtrC object) {

	ScriptVarPtr left = this->GetVariable();
	ScriptVarPtr right = object->GetVariable();

	if (!object->CheckType(Type::Variable)) {
		return ScriptError::Object_UnableToCast;
	}

	if (left->varType == right->varType) {
		return ScriptError::NoError;
	}

	if (left->varType != Var::Object &&
		right->varType != Var::Object
	) {
		this->Clone(object);
	}
	else {
		this->MakeObject(object->GetClass());
	}

	return ScriptError::NoError;
}

Void ScriptVar::Trace(Uint32 offset) {

	if (this->GetClass()) {
		printf("%s %s = ", this->GetClass()->GetName().data(), this->GetName().data());
	} else {
		printf("null %s = ", this->GetName().data());
	}

	if (varType == Var::Float) {
		printf("%.2f", this->v.floatValue);
	}
	else if (varType == Var::Integer) {
		printf("%d", this->v.intValue);
	}
	else {
		if (this->objectValue) {
			this->objectValue->Trace(offset);
		}
		else {
			printf("null");
		}
	}
}

LAME_END2
