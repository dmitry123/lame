#include "Variable.h"
#include "Internal.h"
#include "Lex.h"

LAME_BEGIN2(Script)

Variable::Variable(BufferRefC name, ScopePtr parent, ClassPtr classType) : Object(name, parent, Type::Variable),
	classType(classType)
{
	this->objectValue = NULL;
	this->registerType = NULL;

	if (classType->IsFloat()) {
		this->varType = Var::Float;
	}
	else if (classType->IsInt()) {
		this->varType = Var::Integer;
	}
	else if (classType->IsString()) {
		this->varType = Var::String;
	}
	else {
		this->varType = Var::Object;
	}

	this->v.intValue = 0;
	this->objectValue = 0;
}

Variable::~Variable() {

	if (this->objectValue != NULL && this->objectValue->DecRef()) {
		delete this->objectValue;
	}
}

VariablePtr Variable::SetInteger(ScriptNativeInt i) {

	this->v.intValue = i;
	this->varType = Var::Integer;

	return this;
}

VariablePtr Variable::SetFloat(ScriptNativeFloat f) {

	this->v.floatValue = f;
	this->varType = Var::Float;

	return this;
}

VariablePtr Variable::SetObject(ClassPtr c) {

	if (this->objectValue) {
		if (this->objectValue->DecRef()) {
			delete this->objectValue;
		}
	}

	if (c != NULL) {
		c->IncRef();
	}

	this->objectValue = c;
	this->varType = Var::Object;

	return this;
}

VariablePtr Variable::SetString(ScriptNativeString s) {

	this->stringValue = s;
	this->varType = Var::String;

	return this;
}

ScriptNativeInt Variable::GetInteger(Void) {

	if (this->varType == Var::Float) {
		return (ScriptNativeInt) this->v.floatValue;
	}

	if (this->varType == Var::Object) {
		throw ClassInvalidCastException("Invalid class cast");
	}

	return this->v.intValue;
}

ScriptNativeFloat Variable::GetFloat(Void) {

	if (this->varType == Var::Float) {
		return (ScriptNativeFloat) this->v.intValue;
	}

	if (this->varType == Var::Object) {
		throw ClassInvalidCastException("Invalid class cast");
	}

	return this->v.floatValue;
}

Bool Variable::Equal(ObjectPtrC object) {

	if (this == object) {
		return TRUE;
	}
	if (this->GetType() != object->GetType() ||
		this->varType != object->GetVariable()->varType ||
		this->classType->Hash() != object->GetVariable()->classType->Hash()
	) {
		return FALSE;
	}
	if (this->varType == Var::Integer) {
		return this->v.intValue == object->GetVariable()->v.intValue;
	}
	else if (this->varType == Var::Float) {
		return this->v.floatValue == object->GetVariable()->v.floatValue;
	}
	else {
		return this->objectValue == object->GetVariable()->objectValue;
	}
}

ObjectPtr Variable::Clone(BufferRefC name) {

	VariablePtr newVariable = new Variable(name, this, this->GetClassType());

	if (this->varType == Var::Object) {
		newVariable->SetObject(this->objectValue);
	}
	else if (this->varType == Var::Float) {
		newVariable->SetFloat(this->v.floatValue);
	}
	else {
		newVariable->SetInteger(this->v.intValue);
	}

	return newVariable;
}

Void Variable::Trace(Uint32 offset) {

	this->PrintModificators();

	if (this->GetClass()) {
		if (this->GetName().size() > 0) {
			if (this->GetTemplateClass()) {
				printf("%s<%s> %s = ",
					this->GetClass()->GetName().data(),
					this->GetTemplateClass()->GetName().data(),
					this->GetName().data());
			}
			else {
				printf("%s %s = ", this->GetClass()->GetName().data(), this->GetName().data());
			}
		}
		else {
			printf("%s = ", this->GetClass()->GetName().data());
		}
	}
	else {
		printf("null %s = ", this->GetName().data());
	}

	if (varType == Var::Float) {
		printf("%.2f", this->v.floatValue);
	}
	else if (varType == Var::Integer) {
		if (sizeof(this->v.intValue) == 4) {
			printf("%lld", this->v.intValue);
		}
		else {
			printf("%lld", this->v.intValue);
		}
	}
	else {
		if (this->objectValue && this->GetName() != "this") {
			this->objectValue->Trace(offset);
		}
		else {
			if (this->varType == Var::Float) {
				printf("%.2f", this->v.floatValue);
			}
			else if (this->varType == Var::Integer) {
				printf("%lld", this->v.intValue);
			}
			else if (this->varType == Var::String) {
				printf("\"%s\"", this->stringValue.data());
			}
			else if (this->varType == Var::Object) {
				printf("null");
			}
		}
	}
}

Variable::HashType Variable::Hash(Void) {
	return this->GetPathHash64();
}

Uint32 Variable::Size(Void) {

	if (this->varType == Var::Integer) {
		return sizeof(ScriptNativeInt);
	}
	else if (this->varType == Var::Float) {
		return sizeof(ScriptNativeFloat);
	}
	else {
		return Object::SizeOf;
	}
}

Void Variable::Release(Void) {

	if (this->varType == Var::Object && this->objectValue) {
		this->objectValue->Release();
	}
}

ClassPtr Variable::GetClass() {
	if (this->objectValue) {
		return this->objectValue;
	}
	return this->classType;
}

VariablePtr Variable::GetVariable() {
	return this;
}

Void Variable::Make(Class::Operator command, VariablePtr left, VariablePtr right) {

	if (left->GetClassType()->priority > right->GetClassType()->priority) {
		if (!left->GetClassType()->operators[(Uint32)command]) {
			throw ScriptException("Class operator not overloaded");
		}
		left->GetClassType()->operators[(Uint32)command](this, left, right);
	}
	else {
		if (!right->GetClassType()->operators[(Uint32)command]) {
			throw ScriptException("Class operator not overloaded");
		}
		right->GetClassType()->operators[(Uint32)command](this, left, right);
	}
}

LAME_END2
